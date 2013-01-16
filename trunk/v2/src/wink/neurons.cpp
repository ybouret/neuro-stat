#include "./neurons.hpp"
#include "../pyck/wtime.hpp"

#include <iostream>

namespace wink
{
    
    
    Mutex    neurons::access;
    uint32_t neurons::seed32;
    
    neurons:: ~neurons() throw()
    {
    }
    
    
    
    neurons::neurons() throw() :
    ran()
    {
        PYCK_LOCK(access);
        const uint32_t s32 = IntegerHash32( WallTime::Seed() + seed32++ );
        std::cerr << "s32=" << s32 << std::endl;
        ran.seed(s32);
    }
    
}
