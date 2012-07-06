//! \file
/**
 \brief some utility functions.
 */

#ifndef WINK_UTILS_INCLUDED
#define WINK_UTILS_INCLUDED 1


#include <stddef.h>

namespace wink 
{
    //! sort by increasing order
    /**
     \param x an array of reals
     \param n x[0..n-1]
     */
    void   sort_array( double *x, size_t n );
    
    //! sort by increasing order
    /**
     \param x an array of indices
     \param n x[0..n-1]
     */
    void   sort_indices( size_t *x, size_t n );
    
}

#endif
