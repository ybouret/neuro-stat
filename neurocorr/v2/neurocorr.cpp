#include "yocto/R/R++.hpp"
#include "boxes.hpp"
#include "yocto/rtld/export.hpp"

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
    Rprintf("Entering NeuroCorr\n");
}

static inline void LeaveModule() throw()
{
    Rprintf("Leaving NeuroCorr\n");
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
            team = new Crew(numProcs);
            std::cerr << "+Team@" << (void*)team << std::endl;
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
    Rprintf("Compiled on %s\n", __DATE__);
    return R_NilValue;
}


//______________________________________________________________________________
//
//
// Prototype to see if data is well loaded
//
//______________________________________________________________________________
static inline
Records *BuildRecords(SEXP &RND, SEXP &RNumNeurons, SEXP &RScale)
{
    //______________________________________________________________________
    //
    // Parse Arguments
    //______________________________________________________________________
    RMatrix<Real> neurodata(RND);
    const int     num_neurons = R2Scalar<int>(RNumNeurons);
    const Real    scale       = R2Scalar<Real>(RScale);
    
    //______________________________________________________________________
    //
    // Direct Call
    //______________________________________________________________________
    return new Records(scale,neurodata,num_neurons);
}

extern "C"
SEXP NeuroCorr_CheckNeuroData(SEXP RND, SEXP RNumNeurons, SEXP RScale) throw()
{
    YOCTO_R_PROLOG()
    {
        auto_ptr<Records> pRecords( BuildRecords(RND,RNumNeurons,RScale) );
        Records &records = *pRecords;
        Rprintf("#neurons=%u (#trials=%u)\n", unsigned(records.neurones), unsigned(records.trials) );
        for(size_t i=0; i<records.neurones; ++i)
        {
            Rprintf("Neuron[%3d]:\n", i);
            for(unsigned j=0; j<records.trials; ++j)
            {
                const Train &tr = *records[j][i];
                Rprintf("\ttrial[%3u](#%3u)",j,tr.size());
                Rprintf("\n");
            }
            
        }
        return R_NilValue;
    }
    YOCTO_R_EPILOG()
}
