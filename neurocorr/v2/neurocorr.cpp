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
    Rprintf("Enter NeuroCorr\n");
}

static inline void LeaveModule() throw()
{
    Rprintf("Leave NeuroCorr\n");
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


//______________________________________________________________________________
//
//
// Code to compute matrices
//
//______________________________________________________________________________
static inline
Boxes *BuildBoxes(SEXP        &BoxesR,
                  const Real   scale,
                  const size_t trials)
{
    const RMatrix<double> rawBoxes(BoxesR);
    const size_t nb = rawBoxes.cols;
    const size_t nr = rawBoxes.rows;
    if(nr<4) throw exception("Need at least 4 rows for Boxes!");
    auto_ptr<Boxes> pB( new Boxes(scale,nb) );
    for(size_t b=0;b<nb;++b)
    {
        const RMatrix<double>::Column &Col = rawBoxes[b];

        const size_t raw_trial = int(Col[0]);
        if(raw_trial<=0||raw_trial>trials)
        {
            throw exception("Box[%u]: invalid trial #%u", unsigned(b)+1, unsigned(raw_trial));
        }
        const Unit tauStart = pB->toUnit(Col[1]);
        const Unit tauFinal = pB->toUnit(Col[2]);
        if(tauFinal<=tauStart)
        {
            throw exception("Box[%u]: invalid times (up to scale=%g)", unsigned(b)+1, pB->scale);
        }
        const int kind = int(Col[3]);
        const Box box(raw_trial-1,tauStart,tauFinal,kind);
        pB->push_back(box);
    }
    return pB.yield();
}

#include <cstring>

static inline
Grouping GetGroupingFrom( SEXP &GroupingR )
{
    const char *grp = R2String(GroupingR);
    Rprintf("[NeuroCorr] Grouping is %s\n", grp);

    if( 0==strcmp(grp,"byKind") )
        return GroupByKind;

    if( 0==strcmp(grp,"byBox") )
        return GroupByBox;

    throw exception("[NeuroCorr] Unknown Grouping '%s'",grp);
}

extern "C"
SEXP NeuroCorr_Compute(SEXP neuroDataR,
                       SEXP numNeuronesR,
                       SEXP scaleR,
                       SEXP deltaR,
                       SEXP KR,
                       SEXP BoxesR,
                       SEXP GroupingR)
{
    YOCTO_R_PROLOG()
    {

        //______________________________________________________________________
        //
        // Get Grouping
        //______________________________________________________________________
        const Grouping grouping = GetGroupingFrom(GroupingR);

        //______________________________________________________________________
        //
        // Prepare Records
        //______________________________________________________________________
        Rprintf("[NeuroCorr] building records\n");
        auto_ptr<Records> pRecords( BuildRecords(neuroDataR,numNeuronesR,scaleR) );
        Records          &records = *pRecords;
        Rprintf("[NeuroCorr] #neurones=%u, #trials=%u\n", unsigned(records.neurones), unsigned(records.trials));

        //______________________________________________________________________
        //
        // Prepare Boxes
        //______________________________________________________________________
        auto_ptr<Boxes>   pBoxes( BuildBoxes( BoxesR, records.scale, records.trials) );
        Boxes            &boxes = *pBoxes;
        Rprintf("[NeuroCorr] #boxes=%u\n", unsigned(boxes.size) );

        //______________________________________________________________________
        //
        // Compute Phi, using parallel team if possible
        //______________________________________________________________________
        const Unit   delta   = max_of<int>(1, records.toUnit(R2Scalar<double>(deltaR)) );
        const size_t K       = max_of<int>(1, R2Scalar<int>(KR) );
        Rprintf("[NeuroCorr] allocating Phi functions\n");
        PHI          Phi(K-1,records);
        Rprintf("[NeuroCorr] computing Phi functions for K=%u, delta=%u/%g\n", unsigned(K), unsigned(delta), records.scale);
        Phi.compute(delta,team);

        //______________________________________________________________________
        //
        // Prepare Matrices
        //______________________________________________________________________
        

        return R_NilValue;
    }
    YOCTO_R_EPILOG()
}

