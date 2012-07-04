#include "wink.hpp"
#include <time.h>
#include <cassert>

namespace wink
{
    void init_alea()
    {
        srand( time(NULL) );
        for( size_t i=0; i < 128; ++i )
        {
            rand();
        }
    }
    
    double alea()
    {
        static const double factor = 1.0 / ( double(RAND_MAX) + 1 );
        const double ans    = factor * ( 0.5 + double( rand() ) );
        return ans;
    }
    
    size_t random_index( double (*uniform_generator)()  )
    {
        assert(uniform_generator);
        size_t ans = 0;
        for( size_t i=0; i < sizeof(size_t); ++i )
        {
            ans <<= 1;
            ans |= (uniform_generator()>0.5?1:0);
        }
        return ans;
    }
    
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

    
    
    void   fill_alea_array( double *x, size_t n, double a, double b )
    {
        assert(!(x==NULL&&n>0));
        const double l = b-a;
        for( size_t i=0; i < n; ++i )
        {
            x[i] = a + alea() * l;
        }
        sort_array(x,n);
    }
    
}


