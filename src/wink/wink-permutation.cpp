#include "wink.hpp"
#include <time.h>
#include <cassert>
#include <cmath>

namespace wink
{
    void build_identity( size_t *indices, size_t n)
    {
        assert( !(indices==NULL&&n>0) );
        for( size_t i=0; i < n; ++i ) indices[i] = i;
    }
    
    static inline
    size_t __full( double (*uniform_generator)() )
    {
        size_t ans = 0;
        for( size_t i=0; i < sizeof(size_t); ++i )
        {
            ans <<= 1;
            ans |= (uniform_generator()>0.5?1:0);
        }
        return ans;
    }
    
    void build_permutation( size_t *indices, size_t n, double (*uniform_generator)() )
    {
        build_identity(indices, n);
        if(n>1)
        {
            for( size_t i=n-1;i>0;--i)
            {
                const size_t j = __full(uniform_generator) % (i+1);
                const size_t tmp = indices[i];
                indices[i] = indices[j];
                indices[j] = tmp;
            }
        }
    }
    
    
}

