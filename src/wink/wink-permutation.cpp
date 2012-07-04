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
    
  
    
    void build_permutation( size_t *indices, size_t n, double (*uniform_generator)() )
    {
        build_identity(indices, n);
        if(n>1)
        {
            for( size_t i=n-1;i>0;--i)
            {
                const size_t j = random_index(uniform_generator) % (i+1);
                const size_t tmp = indices[i];
                indices[i] = indices[j];
                indices[j] = tmp;
            }
        }
    }
    
    permutation:: ~permutation() throw()
    {
        delete []indices;
    }
    
    permutation:: permutation( size_t n ) :
    size(n),
    indices(0)
    {
        if( size < 1 ) throw "invalid permutation size";
        indices = new size_t[ size ];
        identity();
    }
    
    void permutation::identity() throw() { build_identity(indices, size); }
    void permutation::rebuild( double (*uniform_generator)() ) throw() { build_permutation(indices, size); }
    
}

