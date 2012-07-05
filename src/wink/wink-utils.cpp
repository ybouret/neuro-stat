#include "wink.hpp"
#include <time.h>
#include <assert.h>
#include <stdlib.h>

namespace wink
{
        
    static inline int compare_doubles( const void *lhs, const void *rhs ) throw()
    {
        const double L = *(double *)lhs;
        const double R = *(double *)rhs;
        return L < R ? -1 : ( R < L ? 1 : 0 );
    }
    
    void   sort_array( double *x, size_t n )
    {
        assert(!(x==NULL&&n>0));
        qsort(x, n, sizeof(double), compare_doubles);
    }
    
    static inline int compare_indices( const void *lhs, const void *rhs ) throw()
    {
        const size_t L = *(size_t *)lhs;
        const size_t R = *(size_t *)rhs;
        return L < R ? -1 : ( R < L ? 1 : 0 );
    }
    
    void   sort_indices( size_t *x, size_t n )
    {
        assert(!(x==NULL&&n>0));
        qsort(x, n, sizeof(double), compare_indices);
    }

    
    
       
}


