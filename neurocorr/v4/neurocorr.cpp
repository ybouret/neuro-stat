#include "yocto/string.hpp"
#include "yocto/R++/com.hpp"
#include "vector-builder.hpp"
#include "matrix-builder.hpp"

using namespace yocto;

YOCTO_R_FUNCTION(NeuroCorr_Version,())
{
    Rprintf("%s: compiled on %s\n", __fn, __DATE__ );
    return R_NilValue;
}
YOCTO_R_RETURN()


static size_t NumThreads = 0;

#include "yocto/sys/hw.hpp"

YOCTO_R_FUNCTION(NeuroCorr_SetParallel,(SEXP numProcsR))
{
    NumThreads = 0;
    int numProcs = R2Scalar<int>(numProcsR);
    if(numProcs>0)
    {
        const int maxProcs = hardware::nprocs();
        if(numProcs>maxProcs)
        {
            numProcs=maxProcs;
        }
        NumThreads = numProcs;
    }
    return R_NilValue;

}
YOCTO_R_RETURN()


YOCTO_R_FUNCTION(NeuroCorr_CheckData,
                 (SEXP dataNeurR, SEXP numNeuronesR, SEXP scaleR))
{
    RMatrix<Real> dataNeur(dataNeurR);
    const int     numNeurones = R2Scalar<int>(numNeuronesR);
    const Real    scale       = R2Scalar<double>(scaleR);

    //std::cerr << "#neurones=" << numNeurones << std::endl;
    //std::cerr << "Scale    =" << scale       << std::endl;

    const Records records(scale,dataNeur,numNeurones);
    records.display();

    return R_NilValue;
}
YOCTO_R_RETURN()


YOCTO_R_STATIC
Grouping ParseGroupingFrom( SEXP &groupingR )
{
    const char *grp = R2String(groupingR);
    if(!grp) throw exception("bad grouping string!!!");

    if(0==strcmp(grp,"byKind"))
        return GroupByKind;

    if(0==strcmp(grp,"byBox"))
        return GroupByBox;

    throw exception("invalid grouping '%s'", grp);
}


YOCTO_R_STATIC
void BuildListOfMoments(RList                 &L,
                        const  UMatrices      &mu,
                        const  Real            scale=1.0)
{
    for(size_t m=1;m<=mu.count;++m)
    {
        const matrix_of<Unit> &src = mu[m];
        RMatrix<Real>          tgt(src.rows,src.cols);
        for(size_t c=1;c<=tgt.cols;++c)
        {
            for(size_t r=1;r<=tgt.rows;++r)
            {
                tgt[c][r] = src(r,c)/scale;
            }
        }
        L.set(m-1,tgt);
    }

}


YOCTO_R_FUNCTION(NeuroCorr_Compute,
                 (SEXP dataNeurR,
                  SEXP numNeuronesR,
                  SEXP scaleR,
                  SEXP deltaR,
                  SEXP KR,
                  SEXP BoxesR,
                  SEXP groupingR)
                 )
{
    //__________________________________________________________________________
    //
    // acquire records
    //__________________________________________________________________________
    const RMatrix<Real> dataNeur( dataNeurR );
    const int           numNeurones = R2Scalar<int>(numNeuronesR);
    const Real          scale       = R2Scalar<double>(scaleR);
    if(scale<=0) throw exception("invalid scale=%g!", scale);
    const Records       records(scale,dataNeur,numNeurones);

    //__________________________________________________________________________
    //
    // check delta
    //__________________________________________________________________________
    const Unit          delta       = records.toUnit(R2Scalar<double>(deltaR));
    if(delta<=0) throw exception("invalid delta: check scale or delta value!");

    //__________________________________________________________________________
    //
    // check K
    //__________________________________________________________________________
    const int           K           = R2Scalar<int>(KR);
    if(K<1) throw exception("invalid K=%d", K);

    Rprintf("[%s] #neurones = %u\n",     __fn, unsigned(records.neurones) );
    Rprintf("[%s] #trials   = %u\n",     __fn, unsigned(records.trials)   );
    Rprintf("[%s] scale     = %g\n",     __fn, records.scale);
    Rprintf("[%s] delta     = %ld/%g\n", __fn, long(delta), scale);

    //__________________________________________________________________________
    //
    // check grouping
    //__________________________________________________________________________
    Grouping policy = ParseGroupingFrom(groupingR);
    switch(policy)
    {
        case GroupByKind: Rprintf("[%s] Grouping By Kind\n", __fn); break;
        case GroupByBox:  Rprintf("[%s] Grouping By Box\n" , __fn); break;
    }

    //__________________________________________________________________________
    //
    // Read boxes
    //__________________________________________________________________________
    RMatrix<Real>       rboxes(BoxesR);
    Boxes               boxes(records.scale,rboxes);
    Rprintf("[%s] #Boxes    = %u\n",     __fn, unsigned(boxes.size()));

    for(size_t i=1;i<=boxes.size();++i)
    {
        Rprintf("[%s]           |_kind=%3d, %ld -> %ld\n", __fn, int(boxes[i].kind), long(boxes[i].tauStart), long(boxes[i].tauFinal) );
    }



    //__________________________________________________________________________
    //
    // Build Phi
    //__________________________________________________________________________
    auto_ptr<threading::crew> team( NumThreads>0 ? new threading::crew(NumThreads,0,false) : NULL );
    threading::crew *para = team.__get();

    if(NumThreads)
    {
        Rprintf("[%s] #THREADS  = %u\n", __fn, unsigned(NumThreads) );
    }
    else
    {
        Rprintf("[%s] SEQUENTIAL\n", __fn);
    }
    Rprintf("[%s] Allocating Phi with %d segments\n",     __fn, K);
    PHI Phi(records,K-1);
    Rprintf("[%s] Computing Phi...\n", __fn);
    Phi.build(delta,para);

    //__________________________________________________________________________
    //
    // Prepare Matrices
    //__________________________________________________________________________
    const size_t nm = boxes.assignIndices(policy);
    Rprintf("[%s] #Matrices = %u\n", __fn, unsigned(nm) );
    Rprintf("[%s] dims(G)   = %ux%u\n", __fn, unsigned(Phi.dim), unsigned(Phi.dim));
    UMatrices MuA(nm,Phi.dim,1);
    UMatrices Mu1(nm,Phi.dim,Phi.neurones);
    UMatrices Mu2(nm,Phi.dim,Phi.neurones);
    UMatrices G(nm,Phi.dim,Phi.dim);

    //__________________________________________________________________________
    //
    // compute matrices
    //__________________________________________________________________________
    Rprintf("[%s] Computing Vectors...\n",__fn);
    {
        VectorBuilder vbuild(Mu1,Mu2,MuA,boxes,Phi,para);
    }
    Rprintf("[%s] Computing G...\n",__fn);
    {
        MatrixBuilder mbuild(G,boxes,Phi,para);
    }


    //__________________________________________________________________________
    //
    // Formating Labels
    //__________________________________________________________________________
    static const char *ansNames[] = { "mu1" , "mu2", "muA", "G" };
    RList              ans( ansNames, sizeof(ansNames)/sizeof(ansNames[0]) );

    vector<string>     labels(nm,as_capacity);
    {
        for(size_t sub=1;sub<=nm;++sub)
        {
            const string label = vformat("%u",unsigned(sub));
            labels.push_back(label);
        }
    }

    //__________________________________________________________________________
    //
    // Transfer lists
    //__________________________________________________________________________
    {

        RList mu1_list(labels);
        BuildListOfMoments(mu1_list,Mu1);
        ans.set(0,mu1_list);
    }

    {
        RList mu2_list(labels);
        BuildListOfMoments(mu2_list,Mu2);
        ans.set(1,mu2_list);
    }

    {
        RList muA_list(labels);
        BuildListOfMoments(muA_list,MuA);
        ans.set(2,muA_list);
    }

    {
        RList G_list(labels);
        BuildListOfMoments(G_list,G,records.scale);
        ans.set(3,G_list);
    }



    return *ans;
}
YOCTO_R_RETURN()
