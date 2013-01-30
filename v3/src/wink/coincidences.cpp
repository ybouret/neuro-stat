#include "./coincidences.hpp"
#include <cassert>
#include <cmath>

namespace wink
{
    
    count_t coincidences::count(const double *X,
                                const size_t  Nx,
                                const double *Y,
                                const size_t  Ny,
                                const double  delta) throw()
    {
        assert(Nx>0);
        assert(Ny>0);
        assert(X!=NULL);
        assert(Y!=NULL);
        
        count_t      count = 0;
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
                break; // no count
            
            assert(Y[j]>=Xlo);
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
    
    count_t coincidences::count_all(const double *X,
                                    const size_t  Nx,
                                    const double *Y,
                                    const size_t  Ny,
                                    const double  delta) throw()
    {
        assert(Nx>0);
        assert(Ny>0);
        assert(X!=NULL);
        assert(Y!=NULL);
        count_t count = 0;
        
        for( size_t i=0; i < Nx; ++i )
        {
            const double Xi  = X[i];       // current value
            for(size_t j=0; j < Ny; ++j )
            {
                const double Yj = Y[j];
                if( fabs(Xi-Yj) <= delta ) ++count;
            }
        }
        return count;
    }
    
    
}
