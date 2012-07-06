/**
 \file 
 \brief Lookup indices for windows construction
 */

#ifndef YOCTO_WINK_LOOKUP_INCLUDED
#define YOCTO_WINK_LOOKUP_INCLUDED 1

#include <stddef.h>

namespace wink
{
    
    //! Algorithms to find the proper indices
    struct lookup
    {
        //! find index such that  X[i-1] < a <= X[i]
        /**
         \param a any real
         \param X a record X[0] = Nx, X[1..Nx] are the ORDERED data
         \param i the index
         \return true if this is possible and i is the answer, false otherwise and i=0
         */
        static bool index_after( double a, const double *X, size_t &i );
        
        //! find index such that  X[i] <= b < X[i+1]
        /**
         \param b any real
         \param X a record X[0] = Nx, X[1..Nx] are the ORDERED data
         \param i the index
         \return true if this is possible and i is the answer, false otherwise and i=0
         */
        static bool index_before( double b, const double *X, size_t &i );
    };
    
}


#endif
