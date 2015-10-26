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


YOCTO_R_FUNCTION(NeuroCorr_CheckData,(SEXP dataNeurR, SEXP numNeuronesR, SEXP scaleR) )
{
    RMatrix<Real> dataNeur(dataNeurR);
    const int     numNeurones = R2Scalar<int>(numNeuronesR);
    const Real    scale       = R2Scalar<double>(scaleR);

    std::cerr << "#neurones=" << numNeurones << std::endl;
    std::cerr << "Scale    =" << scale       << std::endl;

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
    auto_ptr<threading::crew> team;
    if(NumThreads>0) team.reset( new threading::crew(NumThreads,0,false) );
    threading::crew *para = team.__get();
    
    Rprintf("[%s] #Threads  = %u\n", __fn, unsigned(NumThreads) );
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
    Rprintf("[%s] Computing Vectors...\n",__fn);
    {
        VectorBuilder vbuild(Mu1,Mu2,MuA,boxes,Phi,para);
    }
    Rprintf("[%s] Computing G...\n",__fn);
    {
        MatrixBuilder mbuild(G,boxes,Phi,para);
    }
    
    return R_NilValue;
}
YOCTO_R_RETURN()
