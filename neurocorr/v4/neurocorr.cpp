#include "yocto/string.hpp"
#include "yocto/R++/com.hpp"
#include "vector-builder.hpp"
#include "matrix-builder.hpp"

using namespace yocto;

////////////////////////////////////////////////////////////////////////////////
//
// Function to detect loading
//
////////////////////////////////////////////////////////////////////////////////
YOCTO_R_FUNCTION(NeuroCorr_Version,())
{
    Rprintf("%s: compiled on %s\n", __fn, __DATE__ );
    try
    {
        throw exception("Testing exception...");
    }
    catch(const exception &e)
    {
        Rprintf("caught '%s'\n", e.what() );
        Rprintf("reason '%s'\n", e.when() );
    }
    return R_NilValue;
}
YOCTO_R_RETURN()


////////////////////////////////////////////////////////////////////////////////
//
// Multithreading settings
//
////////////////////////////////////////////////////////////////////////////////
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


////////////////////////////////////////////////////////////////////////////////
//
// Check format of data
//
////////////////////////////////////////////////////////////////////////////////
YOCTO_R_FUNCTION(NeuroCorr_CheckData,
                 (SEXP dataNeurR, SEXP numNeuronesR, SEXP scaleR))
{
    RMatrix<Real> dataNeur(dataNeurR);
    const int     numNeurones = R2Scalar<int>(numNeuronesR);
    const Real    scale       = R2Scalar<double>(scaleR);


    const Records records(scale,dataNeur,numNeurones);
    records.display();

    return R_NilValue;
}
YOCTO_R_RETURN()

#include "yocto/sequence/vector.hpp"

////////////////////////////////////////////////////////////////////////////////
//
// Compute Phi from a vector of data
//
////////////////////////////////////////////////////////////////////////////////
YOCTO_R_FUNCTION(NeuroCorr_Phi0,
                 (SEXP trainR,SEXP scaleR,SEXP deltaR))
{


    //__________________________________________________________________________
    //
    // get data
    //__________________________________________________________________________
    RVector<Real>       spikes(trainR);
    const Real          scale       = R2Scalar<double>(scaleR);
    if(scale<1)
    {
        throw exception("[%s] invalid scale=%g!", __fn, scale);
    }


    Converter           conv(scale);
    const Unit          delta       = conv.toUnit(R2Scalar<double>(deltaR));
    if(delta<=0)
    {
        throw exception("[%s] invalid delta: check scale or delta value!",__fn);
    }

    //__________________________________________________________________________
    //
    // convert all
    //__________________________________________________________________________
    const size_t ns = spikes.size();
    vector<Unit> train(ns);
    vector<Unit> shift(ns);
    for(size_t i=ns;i>0;--i)
    {
        train[i] = conv.toUnit(spikes[i]);
    }

    //__________________________________________________________________________
    //
    // compute
    //__________________________________________________________________________
    CPW phi;
    phi.buildFrom(train,delta,shift);

    const size_t n = phi.size();
    //__________________________________________________________________________
    //
    // save
    //__________________________________________________________________________

    switch(n)
    {
        case 0:
        {
            RMatrix<Real> M(2,2);
            M[1][1] = conv.toReal(-1); M[1][2] = phi.foot;
            M[2][1] = conv.toReal( 1); M[2][2] = phi.foot;
            return *M;
        }



        case 1:
        {
            RMatrix<Real> M(2,2);
            const coord  &C = phi.front();
            const Real    t = conv.toReal(C.tau);
            M[1][1] = t; M[1][2] = phi.foot;
            M[1][2] = t; M[2][2] = C.value;
            return *M;
        }

        default:
            break;
    }

    RMatrix<Real> M(2*n,2);
    RMatrix<Real>::Column &X = M[1];
    RMatrix<Real>::Column &Y = M[2];
    M.ld(-1);
    size_t r = 0;

    //__________________________________________________________________________
    //
    // first point
    //__________________________________________________________________________
    ++r;
    X[r] = conv.toReal(phi[1].tau); Y[r] = phi.foot;


    //__________________________________________________________________________
    //
    // core
    //__________________________________________________________________________
    for(size_t i=1;i<n;++i)
    {
        ++r;
        X[r] = conv.toReal(phi[i].tau);   Y[r] = phi[i].value;
        ++r;
        X[r] = conv.toReal(phi[i+1].tau); Y[r] = phi[i].value;
    }


    //__________________________________________________________________________
    //
    // last point
    //__________________________________________________________________________
    ++r;
    X[r] = conv.toReal(phi[n].tau); Y[r] = phi[n].value;

    return *M;
}
YOCTO_R_RETURN();


////////////////////////////////////////////////////////////////////////////////
//
// Compute Matrices
//
////////////////////////////////////////////////////////////////////////////////

//==============================================================================
//
// convert string to grouping policy
//
//==============================================================================
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

//==============================================================================
//
// tranfer matrices to R
//
//==============================================================================
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

//==============================================================================
//
// building matrices
//
//==============================================================================
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
    if(scale<1)   throw exception("[%s] invalid scale=%g!", __fn, scale);
    const Records       records(scale,dataNeur,numNeurones);

    //__________________________________________________________________________
    //
    // check delta
    //__________________________________________________________________________
    const Unit          delta       = records.toUnit(R2Scalar<double>(deltaR));
    if(delta<=0) throw exception("[%s] invalid delta: check scale or delta value!",__fn);

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
    Rprintf("[%s] Allocating Phi with %d segments\n", __fn, K);
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


#include "minimiser.hpp"
#include "yocto/math/core/symdiag.hpp"
using namespace math;

//==============================================================================
//
// building matrices
//
//==============================================================================
YOCTO_R_FUNCTION(NeuroCorr_Coeff,
                 (SEXP RG,
                  SEXP RMu1,
                  SEXP RMu2,
                  SEXP RMuA,
                  SEXP Rgam)
                 )
{


    //__________________________________________________________________________
    //
    // Get/Check args..
    //__________________________________________________________________________
    const RMatrix<Real> G( RG );
    const RMatrix<Real> Mu1( RMu1 );
    const RMatrix<Real> Mu2( RMu2 );
    const RMatrix<Real> MuA( RMuA );
    const Real          gam = R2Scalar<Real>( Rgam );

    Rprintf("[%s] Finding coefficients...\n", __fn);

    const size_t dim = G.rows;
    if(G.cols!=dim)
    {
        throw exception("[%s] G is not square", __fn );
    }

    if(Mu1.rows!=dim)
    {
        throw exception("[%s] Mu1.rows mismatch", __fn );
    }

    if(Mu2.rows!=dim)
    {
        throw exception("[%s] Mu2.rows mismatch", __fn );
    }

    if(MuA.rows!=dim)
    {
        throw exception("[%s] MuA.rows mismatch", __fn );
    }

    const size_t neurones = Mu1.cols;
    if(Mu2.cols != neurones)
    {
        throw exception("[%s] Mu2.cols mismatch", __fn );
    }

    if(MuA.cols != 1 )
    {
        throw exception("[%s] MuA.cols!=1", __fn);
    }

    //__________________________________________________________________________
    //
    // Parallelism
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

    //__________________________________________________________________________
    //
    // algebra
    //__________________________________________________________________________
    RMatrix<Real> a(dim,neurones);
    RVector<Real> count(neurones);
    RVector<Real> H(neurones);

    Rprintf("[%s] Computing Pseudo Inverse\n",__fn);
    matrix<Real>  Q(dim,dim);
    matrix<Real>  V(dim,dim);
    vector<Real>  Lam(dim);
    for(size_t i=dim;i>0;--i)
    {
        for(size_t j=dim;j>0;--j)
        {
            Q[i][j] = G[j][i]; // well, symetric, doesn't matter
        }
    }

    if( !symdiag<Real>::build(Q,Lam,V) )
    {
        throw exception("Cannot factorize G");
    }

    std::cerr << "Lam=" << Lam << std::endl;

    const size_t kerDim = symdiag<Real>::eiginv(Lam);
    if(kerDim)
    {
        Rprintf("[%s] |ker(G)|= %u\n", __fn, unsigned(kerDim) );
    }
    else
    {
        Rprintf("[%s] G seems invertible\n", __fn);
    }
    std::cerr << "iLam=" << Lam << std::endl;
    symdiag<Real>::build(Q,Lam,V);
    std::cerr << "G=" << G << ";" << std::endl;
    std::cerr << "Q=" << Q << ";" << std::endl;



    Rprintf("[%s] Minimising Criteria...\n",__fn);
    Minimisers opt(G,Q,Mu1,Mu2,MuA,a,count,H,gam,para);
    opt.run(para);

    static const char *ansNames[] = { "a", "H", "iter" };
    RList              ans( ansNames, sizeof(ansNames)/sizeof(ansNames[0]) );

    ans.set(0,a);
    ans.set(1,H);
    ans.set(2,count);

    return *ans;
}
YOCTO_R_RETURN()

