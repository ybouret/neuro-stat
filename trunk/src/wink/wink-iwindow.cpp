#include "wink.hpp"
#include <cassert>

namespace wink
{
    
    
    void iwindow:: initialize( double a, double b, const double *X )
    {
        assert(a<=b);
        length = 0;
        const bool foundA = find_index_after(a,  X, indexA);
        const bool foundB = find_index_before(b, X, indexB);
        if( foundA && foundB && (indexA <= indexB) )
        {
            length = indexB - indexA + 1;
        }
    }

}

