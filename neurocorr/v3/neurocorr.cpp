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
        Grouping kind = ParseGroupingFrom(groupingR);
        switch(kind)
        {
            case GroupByKind: Rprintf("[NeuroCorr] Compute: Grouping By Kind\n"); break;
            case GroupByBox:  Rprintf("[NeuroCorr] Compute: Grouping By Box\n" ); break;
        }

        //______________________________________________________________________
        //
        // Get The Data
        //______________________________________________________________________
        Rprintf("[NeuroCorr] Compute: Loading Data\n");
        auto_ptr<Records> pR( CreateRecordsFrom(dataNeurR,numNeuronesR,scaleR) );
        const Records &records = *pR;

        //______________________________________________________________________
        //
        // Get The Boxes
        //______________________________________________________________________
        Rprintf("[NeuroCorr] Compute: Loading Boxes\n");
        RMatrix<Real> MatBoxes(BoxesR);
        Boxes boxes(records.scale,MatBoxes);
        for(unsigned i=0;i<boxes.size;++i)
        {
            Rprintf("[NeuroCorr] Compute: box #%u: trial=%u, %ld->%ld, kind=%d\n", i, unsigned(boxes[i].trial), long(boxes[i].tauStart), long(boxes[i].tauFinal), int(boxes[i].kind) );
        }

        //______________________________________________________________________
        //
        // Allocating Phi functions
        //______________________________________________________________________
        const int      K       = R2Scalar<int>(KR);
        if(K<=0) throw exception("[NeuroCorr] Compute: K=%d", K);
        Rprintf("[NeuroCorr] Compute: Allocating Memory for Phi | K=%d\n",K);
        PHI Phi(records,K-1);

        //______________________________________________________________________
        //
        // Computing Phi Functions
        //______________________________________________________________________
        const Unit     delta   = records.toUnit( R2Scalar<Real>(deltaR) );
        if(delta<=0) throw exception("[NeuroCorr] Compute: delta<=0 units");
        Rprintf("[NeuroCorr] Compute: Computing Phi with delta=%ld units, #CPU=%u\n", long(delta),np);
        Phi.compute(delta,team);


        //______________________________________________________________________
        //
        // Computing Matrices
        //______________________________________________________________________



        return R_NilValue;
    }
    YOCTO_R_EPILOG()
}



