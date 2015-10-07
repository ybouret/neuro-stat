#include "yocto/string.hpp"
#include "yocto/R/R++.hpp"
#include "yocto/rtld/export.hpp"
#include "boxes.hpp"

using namespace yocto;

static Crew *team = NULL;


static inline void TeamDelete() throw()
{
    if(team)
    {
        delete team;
        team = NULL;
    }
}

static inline void EnterModule() throw()
{
    Rprintf("[NeuroCorr] Enter\n");
}

static inline void LeaveModule() throw()
{
    Rprintf("[NeuroCorr] Leave\n");
    if(team)
    {
        Rprintf("WARNING: you should call NeuroCorr_CleanUp()");
    }
}



YOCTO_RTLD_SETUP(EnterModule,LeaveModule)

#include "yocto/sys/hw.hpp"

extern "C"
SEXP NeuroCorr_SetParallel( SEXP numProcsR ) throw()
{
    YOCTO_R_PROLOG()
    {
        TeamDelete();
        int numProcs = R2Scalar<int>(numProcsR);
        if(numProcs>0)
        {
            const int maxProcs = hardware::nprocs();
            if(numProcs>maxProcs)
            {
                numProcs=maxProcs;
            }
            team = new Crew(numProcs,0,false);
        }
        return R_NilValue;
    }
    YOCTO_R_EPILOG()
}

extern "C"
SEXP NeuroCorr_CleanUp() throw()
{
    TeamDelete();
    return R_NilValue;
}

//______________________________________________________________________________
//
//
// Used to check I/O
//
//______________________________________________________________________________
extern "C"
SEXP NeuroCorr_Version() throw()
{
    Rprintf("[NeuroCorr] Compiled on %s\n", __DATE__);
    return R_NilValue;
}

//______________________________________________________________________________
//
//
// Load Data
//
//______________________________________________________________________________
static inline
Records *CreateRecordsFrom(SEXP &dataNeurR, SEXP &numNeuronesR, SEXP &scaleR)
{
    RMatrix<double> dataNeur(dataNeurR);
    const int       numNeurones = R2Scalar<int>(numNeuronesR);
    const Real      scale       = R2Scalar<Real>(scaleR);
    if(numNeurones<=0)
        throw exception("[NeuroCorr] numNeurones=%d", numNeurones);
    if(scale<=0)
        throw exception("[NeuroCorr] scale=%g", scale);

    return new Records(scale,dataNeur,numNeurones);
}

extern "C"
SEXP NeuroCorr_CheckData(SEXP dataNeurR, SEXP numNeuronesR, SEXP scaleR) throw()
{
    YOCTO_R_PROLOG()
    {

        auto_ptr<Records> pR( CreateRecordsFrom(dataNeurR,numNeuronesR,scaleR) );
        const Records &records = *pR;
        records.display();
        return R_NilValue;
    }
    YOCTO_R_EPILOG()
}

//______________________________________________________________________________
//
//
// Compute Phi0 for the first train
//
//______________________________________________________________________________
extern "C"
SEXP NeuroCorr_ComputePhi0(SEXP dataNeurR,
                           SEXP numNeuronesR,
                           SEXP scaleR,
                           SEXP deltaR) throw()
{
    YOCTO_R_PROLOG()
    {
        auto_ptr<Records> pR( CreateRecordsFrom(dataNeurR,numNeuronesR,scaleR) );
        const Records &records = *pR;
        const Unit     delta   = records.toUnit( R2Scalar<Real>(deltaR) );
        if(delta<=0) throw exception("[NeuroCorr] ComputePhi0: delta<=0");
        PHI Phi(records,0);
        Phi.compute(delta,team);
        CPW_Functions &Phi0  = Phi[0][0];
        const Train &train   = Phi0.train;
        const size_t trnum   = train.size();

        const char *names[] = { "train", "graph" };
        RList ans(names,sizeof(names)/sizeof(names[0]));

        if(trnum>0)
        {
            RMatrix<Real> tr(trnum,2);
            tr.ld(0);
            for(size_t i=0;i<trnum;++i)
            {
                tr[0][i] = train[i];
            }
            ans.set(0,tr);
        }

        const CPW    &F  = Phi0[0];
        const size_t  np = 2*F.size;
        if(np>0)
        {
            RMatrix<Real> f(np,2);
            f.ld(0);
            f[0][0] = F[0].tau;
            f[1][0] = F.foot;
            for(size_t i=0;i<F.size-1;++i)
            {
                const size_t j=1+i*2;
                f[0][j]   = F[i].tau;   f[1][j]   = F[i].value;
                f[0][j+1] = F[i+1].tau; f[1][j+1] = F[i].value;
            }
            f[0][np-1] = F[F.size-1].tau;
            f[1][np-1] = F[F.size-1].value;
            ans.set(1,f);
        }


        return *ans;
    }
    YOCTO_R_EPILOG()
}

//______________________________________________________________________________
//
//
// Do something
//
//______________________________________________________________________________
static inline
Grouping ParseGroupingFrom( SEXP &groupingR )
{
    const char *grp = R2String(groupingR);
    if(!grp) throw exception("[NeuroCorr] bad grouping string!!!");

    if(0==strcmp(grp,"byKind"))
        return GroupByKind;

    if(0==strcmp(grp,"byBox"))
        return GroupByBox;

    throw exception("[NeuroCorr] invalid grouping '%s'", grp);
}

#include "vector-builder.hpp"
#include "matrix-builder.hpp"

#include "yocto/sequence/vector.hpp"

static inline
void BuildListOfMoments(RList                 &L,
                        const  Matrices<Unit> &mu,
                        const  Real            scale=1.0)
{
    for(size_t m=0;m<mu.count;++m)
    {
        const Matrix<Unit> &src = mu[m];
        RMatrix<Real>       tgt(src.rows,src.cols);
        for(size_t c=0;c<tgt.cols;++c)
        {
            for(size_t r=0;r<tgt.rows;++r)
            {
                tgt[c][r] = src(r,c)/scale;
            }
        }
        L.set(m,tgt);
    }

}

#include "yocto/math/kernel/jacobi.hpp"

extern "C"
SEXP NeuroCorr_Compute(SEXP dataNeurR,
                       SEXP numNeuronesR,
                       SEXP scaleR,
                       SEXP deltaR,
                       SEXP KR,
                       SEXP BoxesR,
                       SEXP groupingR)
{

    YOCTO_R_PROLOG()
    {
        const unsigned np = (team ? team->size : 1);
        //______________________________________________________________________
        //
        // Get The Parameters
        //______________________________________________________________________
        Grouping policy = ParseGroupingFrom(groupingR);
        switch(policy)
        {
            case GroupByKind: Rprintf("[NeuroCorr] Compute | Grouping By Kind\n"); break;
            case GroupByBox:  Rprintf("[NeuroCorr] Compute | Grouping By Box\n" ); break;
        }

        //______________________________________________________________________
        //
        // Get The Data
        //______________________________________________________________________
        Rprintf("[NeuroCorr] Compute | Loading Data\n");
        auto_ptr<Records> pR( CreateRecordsFrom(dataNeurR,numNeuronesR,scaleR) );
        const Records &records = *pR;
        Rprintf("[NeuroCorr] Compute | #neurones=%u, #trials=%u, scale=%g\n", unsigned(records.neurones), unsigned(records.trials), records.scale);

        //______________________________________________________________________
        //
        // Get The Boxes
        //______________________________________________________________________
        Rprintf("[NeuroCorr] Compute | Loading Boxes\n");
        RMatrix<Real> MatBoxes(BoxesR);
        Boxes boxes(records.scale,MatBoxes);
        for(unsigned i=0;i<boxes.size;++i)
        {
            Rprintf("[NeuroCorr] Compute |__box #%u: trial=%u, %ld->%ld, kind=%d\n", i, unsigned(boxes[i].trial), long(boxes[i].tauStart), long(boxes[i].tauFinal), int(boxes[i].kind) );
        }

        //______________________________________________________________________
        //
        // Allocating Phi functions
        //______________________________________________________________________
        const int      K       = R2Scalar<int>(KR);
        if(K<=0) throw exception("[NeuroCorr] Compute K=%d", K);
        Rprintf("[NeuroCorr] Compute | Allocating Memory for Phi | K=%d\n",K);
        PHI Phi(records,K-1);

        //______________________________________________________________________
        //
        // Computing Phi Functions
        //______________________________________________________________________
        const Unit     delta   = records.toUnit( R2Scalar<Real>(deltaR) );
        if(delta<=0) throw exception("[NeuroCorr] Compute: delta<=0 units");
        Rprintf("[NeuroCorr] Compute | Computing Phi with delta=%ld units, #CPU=%u\n", long(delta),np);
        Phi.compute(delta,team);


        //______________________________________________________________________
        //
        // Computing Matrices
        //______________________________________________________________________
        const size_t num_matrices = boxes.assignIndices(policy);
        Rprintf("[NeuroCorr] Compute | Allocating %u Matrices of Moments (%ux%u)\n",unsigned(num_matrices), unsigned(Phi.dim), unsigned(Phi.neurones));
        MatricesOf<Unit,CMatrix> mu1(num_matrices,Phi.dim,Phi.neurones);
        MatricesOf<Unit,CMatrix> mu2(num_matrices,Phi.dim,Phi.neurones);
        MatricesOf<Unit,CMatrix> muA(num_matrices,Phi.dim,Phi.neurones);
        Rprintf("[NeuroCorr] Compute | Computing Matrices of Moments with #CPU=%u\n", np);
        VectorBuilder vbuild(mu1,mu2,muA,boxes,Phi,team);

        Rprintf("[NeuroCorr] Compute | Allocating %u Matrices of Moments (%ux%u)\n",unsigned(num_matrices), unsigned(Phi.dim), unsigned(Phi.dim));
        MatricesOf<Unit,CMatrix> G(num_matrices,Phi.dim,Phi.dim);
        Rprintf("[NeuroCorr] Compute | Computing Main Matrix with #CPU=%u\n", np);
        MatrixBuilder mbuild(G,boxes,Phi,team);


        //______________________________________________________________________
        //
        // Formating...
        //______________________________________________________________________
        static const char *ansNames[] = { "mu1" , "mu2", "muA", "G" };
        RList              ans( ansNames, sizeof(ansNames)/sizeof(ansNames[0]) );

        vector<string>     subNames(num_matrices,as_capacity);
        {
            for(size_t sub=0;sub<num_matrices;++sub)
            {
                const string subName = vformat("%u",unsigned(sub+1));
                subNames.push_back(subName);
            }
        }
        CVector<const char *> labels(num_matrices);
        for(size_t i=0;i<num_matrices;++i)
        {
            labels[i] = &(subNames[i+1][0]);
        }

        //______________________________________________________________________
        //
        // Formatting
        //______________________________________________________________________
        {

            RList mu1_list(labels);
            BuildListOfMoments(mu1_list,mu1);
            ans.set(0,mu1_list);
        }

        {
            RList mu2_list(labels);
            BuildListOfMoments(mu2_list,mu2);
            ans.set(1,mu2_list);
        }

        {
            RList muA_list(labels);
            BuildListOfMoments(muA_list,muA);
            ans.set(2,muA_list);
        }

        {
            RList G_list(labels);
            BuildListOfMoments(G_list,G,records.scale);
            ans.set(3,G_list);
        }

#if 0
        const size_t       n = Phi.dim;
        math::matrix<Real> H(n,n);
        math::matrix<Real> P(n,n);
        vector<Real>       lam(n,0);

        for(size_t count=0;count<G.count;++count)
        {
            const Matrix<Unit> &MG = G[count];
            for(size_t i=0;i<n;++i)
            {
                math::matrix<Real>::row &r = H[i+1];
                for(size_t j=0;j<n;++j)
                {
                    r[j+1] = MG(i,j);
                }
            }
            std::cerr << "G=" << MG << std::endl;
            //std::cerr << "H=" << H << std::endl;
            if(!math::jacobi<Real>::build(H,lam,P))
            {
                throw exception("jacobi failure!");
            }
            math::jacobi<Real>::eigsrt(lam,P);
            std::cerr << "lam=diag(" << lam << ");" << std::endl;
            std::cerr << "P  =" << P << std::endl;
        }
#endif
        
        return *ans;
    }
    YOCTO_R_EPILOG()
}



