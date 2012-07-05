#include "../wink-thread.hpp"
#include "../wink-rand32.hpp"
#include <iostream>
#include <cassert>
#include <cmath>

namespace 
{
    struct ThreadParam
    {
        wink::Mutex *mutex;
        int          tag;
        double       sum;
    };
    
    static 
    void ThreadEngine( void *args )
    {
        assert(args);
        wink::rand32_kiss g;
        ThreadParam &param = *(ThreadParam *)args;
        assert(param.mutex);
        wink::Mutex &mutex = *param.mutex;
        for(int iter=1; iter <= 16; ++iter )
        {
            {
                wink::ScopeLock access_to(mutex);
                fprintf( stderr, "In Thread with Tag=%u: iter=%d\n", param.tag, iter);
            }
            const size_t nops = 500000 + g.less_than(500000);
            for( size_t j=0; j <nops; ++j )
            {
                param.sum += cos(j);
            }
            
        }
        
    }
}

int main(int argc, char *argv[] )
{
    
    try 
    {
        wink::Mutex mutex;
        ThreadParam param[2];
        
        param[0].mutex = &mutex;
        param[1].mutex = &mutex;
        
        param[0].tag = 0;
        param[1].tag = 1;
        
        wink::thread t0( ThreadEngine, &param[0] );
        wink::thread t1( ThreadEngine, &param[1] );
        
        t0.join();
        t1.join();
        
        
        return 0;
    }
    catch(...)
    {
        std::cerr << "Error" << std::endl;
    }
    return 1;
}