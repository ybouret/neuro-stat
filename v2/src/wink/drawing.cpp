#include "./drawing.hpp"

namespace wink
{
    
    drawing:: ~drawing() throw()
    {
        delete []reg;;
    }
    
    drawing:: drawing( size_t n ) :
    trials( n ),
    pairs( trials * trials ),
    reg( new ipair[pairs] )
    {
        ipair *p = reg;
        for( size_t i=0; i < trials; ++i )
        {
            for(size_t j=0; j < trials; ++j )
            {
                p->first  = i;
                p->second = j;
                ++p;
            }
        }
    }
    
    
}
