//! file
#ifndef COINCIDENCE_INCLUDED
#define COINCIDENCE_INCLUDED 1

#include <cstdlib>

//! init a built in uniform random number generator in ]0:1[
void   init_alea();

//! returns a uniform random number generator in ]0:1[
double alea();

//! sort by increasing order
void   sort_array( double *x, size_t n );


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
                    const double  delta);



#endif
