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
// Compute Phi0
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
        const char *names[] = { "train", "graph" };
        RList ans(names,sizeof(names)/sizeof(names[0]));


        return *ans;
    }
    YOCTO_R_EPILOG()
}

