#include "wink.hpp"
#include <time.h>
#include <cassert>
#include <cmath>

namespace wink
{
    
        
    void build_permutation( size_t *indices, size_t n, double (*uniform_generator)() )
    {
        assert( !(indices==NULL&&n>0) );
        for( size_t i=0; i < n; ++i ) indices[i] = i;
        if(n>1)
        {
            for( size_t i=n-1;i>0;--i)
            {
                const size_t j = size_t( floor( (i*uniform_generator()) + 0.5 ));
                assert(j<=i);
                assert(j>=0);
                const size_t tmp = indices[i];
                indices[i] = indices[j];
                indices[j] = tmp;
            }
        }
    }
    
    
}

