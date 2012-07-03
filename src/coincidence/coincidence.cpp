#include "coincidence.hpp"
#include <time.h>
#include <cassert>


void init_alea()
{
    srand( time(NULL) );
}

double alea()
{
    const double factor = 1.0 / ( double(RAND_MAX) + 1 );
    return factor * ( 0.5 + double( rand() ) );
}

static inline int compare_doubles( const void *lhs, const void *rhs )
{
    const double L = *(double *)lhs;
    const double R = *(double *)rhs;
    return L < R ? -1 : ( R < L ? 1 : 0 );
}

void   sort_array( double *X, size_t n )
{
    qsort(X, n, sizeof(double), compare_doubles);
}


size_t coincidences(const double *X,
                    const size_t  Nx, 
                    const double *Y, 
                    const size_t  Ny,
                    const double  delta)
{
    assert(Nx>0);
    assert(Ny>0);
    assert(X!=NULL);
    assert(Y!=NULL);
    size_t       count = 0;
	size_t       j     = 0;              // initial value for Y testing
    
    //==========================================================================
    // outer loop: on each X item
    //==========================================================================
    for( size_t i=0; i < Nx; ++i )
    {
        const double Xi  = X[i];       // current value
        const double Xlo = Xi - delta; // lower bound
        
        //----------------------------------------------------------------------
        // find the first Y value greater
        // than the lower bound
        //----------------------------------------------------------------------
        while( (j<Ny) && (Y[j]<Xlo) ) ++j;
        
        //----------------------------------------------------------------------
        // early stop
        //----------------------------------------------------------------------
        if(j>=Ny)
            break;
        
        //----------------------------------------------------------------------
        // test from j
        //----------------------------------------------------------------------
        const double Xup = Xi + delta; // current upper bound
        size_t       k   = j;
        while( (k<Ny) && (Y[k]<=Xup) )
        {
            ++count;
            ++k;
        }
        
    }
    
    return count;
}
