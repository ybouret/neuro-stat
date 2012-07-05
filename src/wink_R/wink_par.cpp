#include <R.h>
#include <Rmath.h>
#include <Rinternals.h>

#include "../wink/wink.hpp"
#include "../wink/wink-c-matrix.hpp"
#include "../wink/wink-thread.hpp"

#include <iostream>

namespace 
{
    class ThreadData
    {
    public:
        explicit ThreadData( wink::Mutex &synchro,
                            const size_t  ntrials,
                            const double *data1,
                            const size_t  ncol1,
                            const double *data2,
                            const size_t  ncol2,
                            const size_t  B) :
        mutex(synchro),
        N1( data1, ntrials, ncol1 ),
        N2( data2, ntrials, ncol2 ),
        perm(ntrials),
        boot(B),
        Bcoinc(boot.indx),
        Bcount(boot.size),
        thr( Engine, this )
        {
            
        }
        
        wink::Mutex       &mutex;
        wink::neuro_trials N1;
        wink::neuro_trials N2;
        wink::permutation  perm;
        wink::permutation  boot;
        size_t            *Bcoinc;
        const size_t      &Bcount;
        
        wink::thread       thr;
        
    private:
        ThreadData( const ThreadData & );
        ThreadData&operator=(const ThreadData &);
        static 
        void Engine( void *args ) throw()
        {
            ThreadData &data = *(ThreadData *)args;
            {
                wink::ScopeLock access_to( data.mutex );
                std::cerr << "Starting Engine!"<< std::endl;
            }
        }
    };
}

extern "C"
SEXP wink_par( SEXP data1, SEXP data2, SEXP windows, SEXP Rdelta, SEXP RB, SEXP Rnt) throw()
{
    
    //==========================================================================
    // extract parameters from data1
    //==========================================================================
    SEXP Rdim1 = getAttrib(data1, R_DimSymbol);
    const size_t  nrow1 = INTEGER(Rdim1)[0];
    const size_t  ncol1 = INTEGER(Rdim1)[1];
    Rprintf("\t#### N1: nrow=%u, ncol=%u\n", unsigned(nrow1), unsigned(ncol1) );
    
    //==========================================================================
    // extract parameters from data2
    //==========================================================================
    SEXP Rdim2 = getAttrib(data2, R_DimSymbol);
    const size_t  nrow2 = INTEGER(Rdim2)[0];
    const size_t  ncol2 = INTEGER(Rdim2)[1];
    Rprintf("\t#### N2: nrow=%u, ncol=%u\n", unsigned(nrow2), unsigned(ncol2) );
    
    
    if( nrow1 != nrow2 )
    {
        Rprintf("*** Error: different #trials\n");
        return R_NilValue;
    }
    
    //==========================================================================
    // extract parameters from windows
    //==========================================================================
    SEXP RdimW = getAttrib(windows, R_DimSymbol);
    if( 2 != INTEGER(RdimW)[0] )
    {
        Rprintf("*** Error: windows must have 2 rows");
    }
    const size_t  nw = INTEGER(RdimW)[1];
    const double *pw = REAL(windows);
    Rprintf("\t#### number of windows=%u\n",unsigned(nw));
    if( nw <= 0 )
    {
        Rprintf("*** Error: invalid #windows\n");
        return R_NilValue;
    }
    //==========================================================================
    // Get delta value
    //==========================================================================
    Rdelta = coerceVector(Rdelta,REALSXP);
    const double delta = REAL(Rdelta)[0];
    Rprintf("\t#### delta=%g\n",delta);
    
    //==========================================================================
    // Get B Count
    //==========================================================================
    RB = coerceVector(RB, INTSXP);
    const size_t B  = INTEGER(RB)[0];
    Rprintf("\t#### B=%u\n",unsigned(B));
    if( B <= 0 )
    {
        Rprintf("*** Error: invalid B<=0\n");
        return R_NilValue;
    }
    
    //==========================================================================
    // Get num_threads
    //==========================================================================
    Rnt = coerceVector(Rnt,INTSXP);
    size_t num_threads = INTEGER(Rnt)[0];
    Rprintf("\t#### num_threads=%u\n",unsigned(num_threads));
    if(num_threads<=0)
        num_threads = 1;
    if(num_threads>nw)
        num_threads = nw;
    
    
    try
    {
        //======================================================================
        // transform R data intro C++ objects
        //======================================================================
        wink::c_matrix     M1(nrow1,ncol1);
        wink::c_matrix     M2(nrow2,ncol2);
        
        M1.loadR( REAL(data1) );
        M2.loadR( REAL(data2) );
        
        //======================================================================
        // Create Thread Data/Threads
        //======================================================================
        wink::Mutex  mutex;
        ThreadData **ppThreadData = new ThreadData * [num_threads];
        size_t       num_active   = 0;
        try
        {
            for( num_active=0; num_active < num_threads; ++num_active )
            {
                ppThreadData[num_active] = new ThreadData(mutex,
                                                          nrow1,
                                                          M1.data,
                                                          ncol1,
                                                          M2.data,
                                                          ncol2,
                                                          B);
            }
        }
        catch(...)
        {
            //==================================================================
            // delete previously created 
            //==================================================================
            while(num_active>0)
            {
                ThreadData *pThreadData = ppThreadData[--num_active];
                assert(pThreadData!=NULL);
                pThreadData->thr.join();
                delete pThreadData;
            }
            delete [] ppThreadData;
            throw;
        }
        
        //======================================================================
        // Ok, everything is fine, wait for threads
        // TODO: Factorize code
        //======================================================================
        while(num_active>0)
        {
            ThreadData *pThreadData = ppThreadData[--num_active];
            assert(pThreadData!=NULL);
            pThreadData->thr.join();
            delete pThreadData;
        }
        delete [] ppThreadData;
        
        return R_NilValue;
    }
    catch(...)
    {
        Rprintf("Exception in code");
    }
    
    return R_NilValue;
}