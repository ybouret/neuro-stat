#include "../par/wink-thread.hpp"
#include "../wink-rand32.hpp"
#include <iostream>
#include <cassert>
#include <cmath>
#include <cstdio>

namespace 
{
    struct ThreadParam
    {
        wink::mutex *m;
        int          tag;
        double       sum;
    };
    
    static 
    void ThreadEngine( void *args )
    {
        assert(args);
        wink::rand32_kiss g;
        ThreadParam &param = *(ThreadParam *)args;
        assert(param.m);
        wink::mutex &m = *param.m;
        for(int iter=1; iter <= 16; ++iter )
        {
            {
                wink::scope_lock access_to(m);
                fprintf( stderr, "In Thread with Tag=%u: iter=%d\n", param.tag, iter);
				fflush(stderr);
            }
            const size_t nops = 500000 + g.less_than(500000);
            for( size_t j=0; j <nops; ++j )
            {
                param.sum += cos(double(j));
            }
            
        }
        
    }
}

int main(int argc, char *argv[] )
{
    
    try 
    {
        wink::mutex m;
        ThreadParam param[2];
        
        param[0].m = &m;
        param[1].m = &m;
        
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