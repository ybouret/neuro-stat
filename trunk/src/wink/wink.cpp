#include "coincidence.hpp"
#include <time.h>
#include <cassert>
#include <iostream>

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
    //std::cerr << "alea=" << ans << std::endl;
    return ans;
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

bool find_index_after( double a, const double *X, size_t &i )
{
    assert(X!=NULL);
    const size_t Nx = size_t(X[0]);
    
    switch( Nx )
    {
        case 0:
            i=0;
            return false;
            
        case 1:
            if( X[1] >= a )
            {
                i=1;
                return true;
            }
            else
            {
                i=0;
                return false;
            }
            break;
            
        default:
            assert(Nx>=2);
            if( a > X[Nx] )
            {
                //--------------------------------------------------------------
                // trivial case
                //--------------------------------------------------------------
                i=0;
                return false;
            }
            else 
            {
                if( X[1] >= a )
                {
                    //----------------------------------------------------------
                    // another trivial case
                    //----------------------------------------------------------
                    i = 1;
                    return true;
                }
                else 
                {
                    //----------------------------------------------------------
                    // generic case: bisection
                    //----------------------------------------------------------
                    size_t ilo = 1;
                    size_t ihi = Nx;
                    while( ihi-ilo > 1 )
                    {
                        const size_t mid = (ilo+ihi) >> 1;
                        if( X[mid] >= a )
                            ihi = mid;
                        else 
                            ilo = mid;
                    }
                    i = ihi;
                    assert(i>0);
                    assert(a<=X[i]);
                    return true;
                }
                
            }
    }
}


bool find_index_before( double b, const double *X, size_t &i )
{
    
    assert(X!=NULL);
    const size_t Nx = size_t(X[0]);
    
    switch( Nx )
    {
        case 0:
            i=0;
            return false;
            
        case 1:
            if( X[1] <= b )
            {
                i=1;
                return true;
            }
            else
            {
                i=0;
                return false;
            }
            break;
            
        default:
            assert(Nx>=2);
            if( b >= X[Nx] )
            {
                //--------------------------------------------------------------
                // trivial case
                //--------------------------------------------------------------
                i = Nx;
                return true;
            }
            else
            {
                if( b < X[1] )
                {
                    //----------------------------------------------------------
                    // another trivial case
                    //----------------------------------------------------------
                    i=0;
                    return false;
                }
                else 
                {
                    //----------------------------------------------------------
                    // generic case: bisection
                    //----------------------------------------------------------
                    size_t ilo = 1;
                    size_t ihi = Nx;
                    while( ihi-ilo > 1 )
                    {
                        const size_t mid = (ilo+ihi) >> 1;
                        if( X[mid] < b )
                            ilo = mid;
                        else 
                            ihi = mid;
                    }
                    i = ilo;
                    assert(i>0);
                    assert(X[i]<=b);
                    return true;
                }
            }
            return false;
    }
    
}

void IWindow:: initialize( double a, double b, const double *X )
{
    
}


