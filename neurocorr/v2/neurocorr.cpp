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
    const Real    scale       = R2Scalar<double>(RScale);
    
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

//______________________________________________________________________________
//
//
// Prototype to check Phi Functions
//
//______________________________________________________________________________
#include "yocto/code/utils.hpp"

extern "C"
SEXP NeuroCorr_ComputePhi(SEXP neuroDataR, SEXP numNeuronesR, SEXP scaleR, SEXP deltaR, SEXP KR ) throw()
{
    YOCTO_R_PROLOG()
    {
        Rprintf("[ComputePhi]: create records\n");
        auto_ptr<Records> pRecords( BuildRecords(neuroDataR,numNeuronesR,scaleR) );
        
        Records     &records = *pRecords;
        const double scale = records.scale;
        const Unit   delta = max_of<int>(1, records.toUnit(R2Scalar<double>(deltaR)) );
        const size_t K     = max_of<int>(1, R2Scalar<int>(KR) );
        
        Rprintf("[ComputePhi]: allocating memory (K=%u)\n",unsigned(K));
        PHI Phi(K-1,records);
        
        Rprintf("[ComputePhi]: computing with delta=%ld/%g\n",long(delta),scale);
        
        Phi.compute(delta,team);
        
        const CPW_Function &F  = (*Phi[0][0])[0];
        const Train        &tr = Phi[0][0]->train;
        const size_t        np = F.size();
        if(np)
        {
            const char *names[] = { "spikes", "phi0" };
            RList ans(names,sizeof(names)/sizeof(names[0]));
            
            const size_t nt = tr.size();
            RMatrix<double> Tr(nt,2);
            for(size_t i=0;i<nt;++i)
            {
                Tr[0][i] = tr[i]/scale;
                Tr[1][i] = 0;
            }
            
            ans.set(0,Tr);
            
            const size_t    ntot = 2*np;
            RMatrix<double> M(ntot,2);
            
            M[0][0] = F[1].tau/scale; M[1][0] = F.foot;
            for(size_t i=1;i<np;++i)
            {
                const size_t j=1+(i-1)*2;
                M[0][j]   = F[i].tau/scale;   M[1][j]   = F[i].value;
                M[0][j+1] = F[i+1].tau/scale; M[1][j+1] = F[i].value;
            }
            M[0][ntot-1] = F[np].tau/scale; M[1][ntot-1] = F[np].value;
            
            ans.set(1,M);
            
            return *ans;
        }
        else
        {
            return R_NilValue;
        }
    }
    YOCTO_R_EPILOG()
}

