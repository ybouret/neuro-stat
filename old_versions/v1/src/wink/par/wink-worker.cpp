#include "wink-worker.hpp"
#include <new>
#include <cstdlib>
#include <iostream>
#include <cstdio>
#include <ctime>

namespace wink
{
    
    slice::slice(size_t         s_offset, 
                 size_t         s_length,
                 const double  *ab,
                 double        *p,
                 const double   lag,
                 const uint32_t s32) throw() :
    offset( s_offset ),
    length( s_length ),
    windows( ab ),
    pvalues(p),
    delta(lag),
    seed(s32)
    {
        assert(windows!=NULL);
        assert(pvalues!=NULL);
    }
    
    slice:: ~slice() throw()
    {
    }
    
    
#if defined(_MSC_VER)
	// this in constructor
#pragma warning ( disable : 4355 )
#endif
    
    worker:: worker(const c_matrix &M1,
                    const c_matrix &M2, 
                    const size_t    B,
                    size_t          s_offset, 
                    size_t          s_length,
                    const double   *ab,
                    double         *p,
                    const double    lag,
                    const uint32_t  s32,
                    wink::mutex    &synchro,
                    worker::task    todo
                    ) :
    neuro_pair(M1,M2,B),
    slice(s_offset,s_length,ab,p,lag,s32),
    access(synchro),
    job( todo ),
    thr( engine, this )
    {
    }
    
       
    void worker:: engine( void *args ) throw()
    {
        //----------------------------------------------------------------------
        // get worker and initialize it
        //----------------------------------------------------------------------
        worker &self = *(worker *)args;
        self.initialize();
        
        //----------------------------------------------------------------------
        // call the job to do
        //----------------------------------------------------------------------
        worker::task run = self.job; // workaround....
        (self.*run)();
    }
    
    void worker:: initialize() throw()
    {
#if 0
        {
            scope_lock guard( access );
            fprintf(stderr, "\t\tworker @%4u +%4u : seed = 0x%08x\n", unsigned(offset), unsigned(length), seed);
            fflush(stderr);
        }
#endif
        
        //----------------------------------------------------------------------
        // Post-Init
        //----------------------------------------------------------------------
        g.seed( seed );
        
    }
    
    void worker:: compute_pvalues_both() throw()
    {
        
        size_t i = offset;
        for( size_t j=length;j>0;--j,++i)
        {
            const size_t i2 = i*2;
            const size_t i3 = i2+1;
            const double a = windows[ i2 ];
            const double b = windows[ i3 ];
            
            both_pvalues( pvalues[i2], pvalues[i3], a, b, delta);
        }
    }
    
    
    void worker:: compute_pvalues_geq() throw()
    {
        size_t i = offset;
        for( size_t j=length;j>0;--j,++i)
        {
            const size_t i2 = i*2;
            const double a = windows[ 0 + i2 ];
            const double b = windows[ 1 + i2 ];
            
            pvalues[i] = pvalue_geq(a, b, delta);
        }
    }
    
    
    worker:: ~worker() throw()
    {
    }
    
    
    workers:: workers(const c_matrix &M1,
                      const c_matrix &M2, 
                      const size_t    B,
                      const size_t    num_threads,
                      const size_t    num_windows,
                      const double   *ab,
                      double         *p,
                      const double    lag,
                      worker::task    todo) :
    maxi( num_threads ),
    size(0),
    crew( (worker *) calloc( maxi, sizeof(worker) ) ),
    access()
    {
        if( !crew )
            throw "no memory for crew";
        
        try 
        {
            size_t offset = 0;
            size_t length = num_windows;
            while(size<maxi)
            {
                //-- compute the work load for this worker
                const size_t   number = length / (maxi-size);
                
                //-- compute a dedicated random seed
				const uint32_t r32    = rand32::ih32( wink::neuro_pair::shared_seed + uint32_t(time(NULL)) );
                const uint32_t s32    = rand32::ih32( r32 + rand32::ih32(size) );
                
                new( &crew[size] ) worker(M1, 
                                          M2,
                                          B,
                                          offset, 
                                          number, 
                                          ab,
                                          p, 
                                          lag, 
                                          s32, 
                                          access, 
                                          todo);
                ++size;
                offset += number;
                length -= number;
            }
        }
        catch(...)
        {
            wait();
            kill();
            throw;
        }
    }
    
    void workers:: wait() throw()
    {
        for( size_t i=0; i < size; ++i )
        {
            crew[i].thr.join();
        }
    }
    
    workers:: ~workers() throw()
    {
        kill();
    }
    
    void workers:: kill() throw()
    {
        while( size > 0 )
        {
            worker *w = &crew[--size];
            w->~worker();
        }
        free( crew );
    }
    
}
