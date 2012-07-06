#include "wink-worker.hpp"
#include <new>
#include <cstdlib>
#include <iostream>

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
    
    
    
    worker:: worker(const c_matrix &M1,
                    const c_matrix &M2, 
                    const size_t    B,
                    size_t          s_offset, 
                    size_t          s_length,
                    const double   *ab,
                    double         *p,
                    const double    lag,
                    const uint32_t  s32,
                    wink::mutex    &synchro
                    ) :
    neuro_pair(M1,M2,B),
    slice(s_offset,s_length,ab,p,lag,s32),
    access(synchro),
    thr( engine, this )
    {
    }
    
    
    void worker:: engine( void *args ) throw()
    {
        worker &self = *(worker *)args;
        self.run();        
    }
    
    void worker:: run() throw()
    {
        {
            scope_lock guard( access );
            std::cerr << "worker @" << offset << " +" << length << std::endl;
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
                      const double    lag) :
    maxi( num_threads ),
    size(0),
    crew( (worker *) calloc( maxi, sizeof(worker) ) ),
    access()
    {
        if( !crew )
            throw "no memory for crew";
        
        try 
        {
            const uint32_t r32 = rand32::ih32( time(NULL) );
            size_t offset = 0;
            size_t length = num_windows;
            while(size<maxi)
            {
                const size_t   todo = length / (maxi-size);
                const uint32_t s32  = rand32::ih32( r32 + rand32::ih32(size) );
                
                new( &crew[size] ) worker(M1, M2, B, offset, todo, ab, p, lag, s32, access);
                
                offset += todo;
                length -= todo;
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
