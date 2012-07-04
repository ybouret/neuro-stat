//! file
/**
 Wink Is a NeuroStats Kernel.
 */
#ifndef WINK_INCLUDED
#define WINK_INCLUDED 1

#include <cstdlib>

namespace wink
{
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
    
    //! fill array with alea value
    void   fill_alea_array( double *x, size_t n, double a, double b );
    
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
    struct iwindow
    {
        size_t indexA; //!< first index
        size_t indexB; //!< last  index
        size_t length; //!< indexB - indexA  +1, 0 is no valid data
        
        //! get indices from data
        /**
         for length to be >0, find_index_after(a,X,indexA) must succeed
         and find_index_before(b,X,indexB) must succeed as weel.
         if( indexA > indexB ), this means that [a,b] is
         between two consecutive X, so the window is empty.
         */
        void   initialize( double a, double b, const double *X );
    };
    
    //! build a random permutation
    /**
     \param indices will contained the permutted indices
     \param num indices[0..num]
     \param uniform_generator in [0:1]
     */
    void build_permutation( size_t *indices, size_t num, double (*uniform_generator)() = wink::alea );
    
    //! build an identity vector
    void build_identity( size_t *indices, size_t num );
    
    
}

#endif
