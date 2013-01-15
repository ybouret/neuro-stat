/**
 \file
 \brief Coincidences detection.
 */
#ifndef WINK_COINCIDENCES_INCLUDED
#define WINK_COINCIDENCES_INCLUDED 1

#include <stddef.h>

namespace wink
{
    struct coincidences
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
        static
        size_t count(const double *X,
                     const size_t  Nx,
                     const double *Y,
                     const size_t  Ny,
                     const double  delta) throw();
    };
    
}

#endif
