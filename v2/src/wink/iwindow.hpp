/**
 \file
 \brief Indices windows.
 */
#ifndef WINK_IWINDOW_INCLUDED
#define WINK_IWINDOW_INCLUDED 1

#include <stddef.h>

namespace wink 
{
    //! location of data
    class iwindow
    {
    public:
        iwindow() throw();
        ~iwindow() throw();
        size_t indexA; //!< first index
        size_t indexB; //!< last  index
        size_t length; //!< indexB - indexA  +1, 0 is no valid data
        
        //! get indices from data
        /**
         for length to be >0, find_index_after(a,X,indexA) must succeed
         and find_index_before(b,X,indexB) must succeed as weel.
         if( indexA > indexB ), this means that [a,b] is
         between two consecutive X, so that the window is empty.
         */
        void   initialize( double a, double b, const double *X );
        
    private:
        iwindow( const iwindow & );
        iwindow & operator=( const iwindow & );
    };
    
    //! detect coincidences
    /**
     \param X a record X[0] = Nx, X[1..Nx] are the ORDERED data
     \param Wx a valid window associated to X
     \param Y a record Y[0] = Ny, Y[1..Ny] are the ORDERED data
     \param Wy a valid window associated to Y
     \return the number of coincidences within lag delta
     */
    size_t coincidences(const double   *X, 
                        const iwindow  &Wx,
                        const double   *Y,
                        const iwindow  &Wy,
                        const double    delta) throw();
    


}

#endif
