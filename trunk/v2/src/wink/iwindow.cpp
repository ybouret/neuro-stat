#include "./iwindow.hpp"
#include "./lookup.hpp"
#include "./coincidences.hpp"
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
        const bool foundA = lookup::index_after( a, X, indexA);
        const bool foundB = lookup::index_before(b, X, indexB);
        if( foundA && foundB && (indexA <= indexB) )
        {
            length = indexB - indexA + 1;
        }
    }
    
    size_t count_coincidences(const double   *X,
                              const iwindow  &Wx,
                              const double   *Y,
                              const iwindow  &Wy,
                              const double    delta) throw()
    {
        assert(X!=NULL);
        assert(Y!=NULL);
        if( Wx.length>0 && Wy.length>0 )
        {
            assert( Wx.indexA>0 );
            assert( Wx.indexB>0 );
            assert( Wx.indexA <= size_t(X[0]) );
            assert( Wx.indexB <= size_t(X[0]) );
            
            assert( Wy.indexA>0 );
            assert( Wy.indexB>0 );
            assert( Wy.indexA <= size_t(Y[0]) );
            assert( Wy.indexB <= size_t(Y[0]) );
            return coincidences::count(X+Wx.indexA, Wx.length, Y+Wy.indexA, Wy.length, delta);
        }
        else
        {
            return 0; //! no data in at least one window
        }
    }
    
}

