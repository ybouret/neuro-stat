//! \file
/**
 \brief coincidences detection
 */
#ifndef WINK_COINCIDENCE_INCLUDED 
#define WINK_COINCIDENCE_INCLUDED 1

#include <stddef.h>

namespace wink 
{
    //! detect coincidences
    /**
     \param X an array of ordered time values
     \param Nx X[0..Nx-1]
     \param Y an array of ordered time values
     \param Ny Y[0..Ny-1]
     \param delta the time lag for detection
     \return the number of |X_i-Y_j| <= delta
     */
    size_t coincidences(const double *X, 
                        const size_t  Nx, 
                        const double *Y, 
                        const size_t  Ny, 
                        const double  delta) throw();
    
}

#endif
