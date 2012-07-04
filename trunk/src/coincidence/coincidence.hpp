//! file
#ifndef COINCIDENCE_INCLUDED
#define COINCIDENCE_INCLUDED 1

#include <cstdlib>

//! init a built in uniform random number generator in ]0:1[
void   init_alea();

//! returns a uniform random number generator in ]0:1[
double alea();

//! sort by increasing order
/**
 \param x an array of reals
 \param n x[0..n-1]
 */
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


//! find index such that  X[i-1] < a <= X[i]
/**
 \param a any real
 \param X a record X[0] = Nx, X[1..Nx] are the ORDERED data
 \param i the index
 \return true if this is possible and i is the answer, false otherwise and i=0
 */
bool find_index_after( double a, const double *X, size_t &i );

//! find index such that  X[i] <= b < X[i+1]
/**
 \param b any real
 \param X a record X[0] = Nx, X[1..Nx] are the ORDERED data
 \param i the index
 \return true if this is possible and i is the answer, false otherwise and i=0
 */
bool find_index_before( double b, const double *X, size_t &i );

//! location of data
struct IWindow
{
    size_t indexA; //!< first index
    size_t indexB; //!< last  index
    size_t length; //!< indexB - indexA  +1, 0 is no valid data
    void   initialize( double a, double b, const double *X );
};

#endif
