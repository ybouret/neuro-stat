#include "wink.hpp"
#include <cassert>

namespace wink
{
    
    iwindow:: iwindow() throw() : indexA(0), indexB(0), length(0) {}
    iwindow:: ~iwindow() throw() {}
    
    void iwindow:: initialize( double a, double b, const double *X )
    {
        assert(a<=b);
        assert(X!=NULL);
        length = 0;
        const bool foundA = find_index_after(a,  X, indexA);
        const bool foundB = find_index_before(b, X, indexB);
        if( foundA && foundB && (indexA <= indexB) )
        {
            length = indexB - indexA + 1;
        }
    }
    
    
    size_t coincidences(const double   *X, 
                        const iwindow  &Wx,
                        const double   *Y,
                        const iwindow  &Wy,
                        const double    delta)
    {
        assert(X!=NULL);
        assert(Y!=NULL);
        if( Wx.length>0 && Wy.length>0 )
        {
            const size_t Nx = size_t(X[0]);
            assert( Wx.indexA>0 );
            assert( Wx.indexB>0 );
            assert( Wx.indexA <= Nx );
            assert( Wx.indexB <= Nx );
            
            const size_t Ny = size_t(Y[0]);
            assert( Wy.indexA>0 );
            assert( Wy.indexB>0 );
            assert( Wy.indexA <= Ny );
            assert( Wy.indexB <= Ny );
            return coincidences(X+Wx.indexA, Wx.length, Y+Wy.indexA, Wy.length, delta);
        }
        else 
        {
            return 0; //! no data in at least one window
        }
    }

}

