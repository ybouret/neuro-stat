#include <R.h>
#include <Rmath.h>
#include <Rinternals.h>

#include "../wink/wink-neuro-pair.hpp"
#include "../wink/par/wink-thread.hpp"

#include <iostream>
#include <time.h>

namespace 
{
    
    //! portion of work to do
    struct Slice
    {
        size_t        offset;
        size_t        length;
        const double *r_windows;
        double       *r_pvalues;
        uint32_t      seed;
    };
    
    //! compute and holds slices 
    class Slices
    {
    public:
        Slices(size_t        num_threads, 
               size_t        num_windows,
               const double *r_windows,
               double       *r_pvalues) :
        count(num_threads),
        slice( new Slice[count] )
        {
            assert(num_threads>0);
            assert(num_threads<=num_windows);
            assert(r_windows!=NULL);
            assert(r_pvalues!=NULL);
            size_t offset = 0;
            size_t remain = num_windows;
            for( size_t i=0; i < count; ++i )
            {
                Slice &s = slice[i];
                const size_t todo = remain / (count-i);
                s.offset = offset;
                s.length = todo;
                s.r_windows = r_windows;
                s.r_pvalues = r_pvalues;
                s.seed      = wink::rand32::ih32(wink::rand32::ih32(time(NULL)) + i);
                remain -= todo;
                offset += todo;
            }
        }
        
        ~Slices() throw()
        {
            delete []slice;
        }
        
        const size_t count; 
        Slice       *slice;
        
    private:
        Slices( const Slices & );
        Slices&operator=(const Slices & );
    };
    
    class Worker : public wink::neuro_pair
    {
    public:
        wink::mutex &guard;
        const Slice &slice;
        const double delta;
        wink::thread thr;
        
        Worker(wink::mutex         &synchro,
               const wink::c_matrix &M1, 
               const wink::c_matrix &M2, 
               const size_t          B,
               const Slice          &S,
               const double          lag) :
        wink::neuro_pair(M1,M2,B),
        guard( synchro ),
        slice(S),
        delta(lag),
        thr( Engine, this )
        {
            
        }
        
        virtual ~Worker() throw() {}
        
        inline void run() throw()
        {
            const double *w = slice.r_windows;
            double       *p = slice.r_pvalues;
            
            size_t i = slice.offset;
            for(size_t j=slice.length;j>0;--j,++i)
            {
                const double a = w[0+i*2];
                const double b = w[1+i*2];
                p[i] = pvalue(a,b,delta);
                //wink::ScopeLock access( mutex );
                //Rprintf("[%10.6f,%10.6f] :  true_coinc=%6u : pvalue= %.8f\n",a,b,true_coinc,p[i]);
            }
        }
        
    private:
        Worker(const Worker & );
        Worker&operator=(const Worker & );
        static void Engine( void *args ) throw()
        {
            assert( args != NULL );
            Worker &w = *(Worker *)args;
            //------------------------------------------------------------------
            // post-init
            //------------------------------------------------------------------
            w.g.seed( w.slice.seed );
            if(false)
            {
                wink::scope_lock access( w.guard );
                Rprintf("Starting engine!\n");
                std::cerr << "\t\tWorker will do " << w.slice.offset << "+" << w.slice.length << " : seed=" << w.slice.seed << std::endl;
            }
            //------------------------------------------------------------------
            // just run...
            //------------------------------------------------------------------
            w.run();
        }
    };
    
    class Workers
    {
    public:
        const size_t maxi;   //!< reserved count
        size_t       size;   //!< actually online
        Worker      *worker; //!< memory
        
        Workers(const size_t          num_threads,
                wink::mutex          &synchro,
                const wink::c_matrix &M1, 
                const wink::c_matrix &M2, 
                const size_t          B,
                const Slices         &S,
                const double          delta) :
        maxi(num_threads),
        size(0),
        worker( (Worker *) calloc( maxi, sizeof(Worker) ) )
        {
            assert( S.count == num_threads );
            std::cerr << "\tCreating Crew" << std::endl;
            if( !worker )
                throw "no memory for parallel Worker";
            try
            {
                while(size<maxi)
                {
                    new (&worker[size]) Worker(synchro,M1,M2,B,S.slice[size],delta);
                    ++size;
                }
            }
            catch(...)
            {
                for(size_t i=0; i < size; ++i ) worker[i].thr.join();
                kill();
                throw;
            }
        }
        
        ~Workers() throw()
        {
            std::cerr << "\tDeleting Crew" << std::endl;
            kill();
        }
        
    private:
        inline void kill() throw()
        {
            while( size> 0 )
            {
                Worker *w = &worker[--size];
                w->~Worker();
            }
            free(worker);
        }
        Workers( const Workers & );
        Workers&operator=(const Workers & );
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
    const size_t  nwindows = INTEGER(RdimW)[1];
    const double *pwindows = REAL(windows);
    Rprintf("\t#### number of windows=%u\n",unsigned(nwindows));
    if( nwindows <= 0 )
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
    if(num_threads>nwindows)
        num_threads = nwindows;
    
    
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
        // create the return vector
        //======================================================================
        SEXP Rval;
        PROTECT(Rval = allocVector(REALSXP,nwindows) );
        double *pvalues = REAL(Rval);
        //memset(pvalues,0,nwindows*sizeof(double));
        
        //======================================================================
        // Create the Crew
        //======================================================================
        wink::mutex  guard;
        Slices       slices( num_threads, nwindows, pwindows,pvalues);
        Workers      crew( num_threads,guard,M1,M2,B,slices,delta);
        
        for( size_t i=0; i < num_threads; ++i ) 
        {
            crew.worker[i].thr.join();
        }
        UNPROTECT(1);
        return Rval;
    }
    catch(...)
    {
        Rprintf("Exception in code");
    }
    
    return R_NilValue;
}
