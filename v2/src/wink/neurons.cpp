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
    drawing(),
    ran()
    {
        PYCK_LOCK(access);
        const uint32_t s32 = IntegerHash32( WallTime::Seed() + seed32++ );
        std::cerr << "s32=" << s32 << std::endl;
        ran.seed(s32);
    }
 
   
    size_t neurons:: total_coincidences( const neuron &N1, const neuron &N2, double delta ) const
    {
        size_t       count  = 0;
        for( const couple *c = head;c;c=c->next)
        {
            assert(c->first  < N1.trials);
            assert(c->second < N2.trials);
            const record &r1 = N1[ c->first  ];
            const record &r2 = N2[ c->second ];
            count += r1.coincidences_with(r2, delta);
        }
        return count;
    }
    

}
