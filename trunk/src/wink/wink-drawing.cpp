#include "wink-drawing.hpp"

namespace wink
{
    
    
    drawing:: ~drawing() throw()
    {
        delete []indx;
    }
    
    drawing:: drawing( size_t n ) :
    size(n),
    indx(0),
    mode( do_shuffle )
    {
        if( size < 1 ) throw "invalid drawing size";
        indx = new size_t[ size ];
        identity();
    }
    
    void drawing::identity() throw()
    {
        assert(indx!=0);
        assert(size>0);
        for( size_t i=0; i < size; ++i ) indx[i] = i;
        
    }
    
    void drawing::generate( urand32 &g ) throw()
    {
        switch(mode)
        {
            case do_shuffle:
                identity();
                g.shuffle<size_t>(indx,size);
                break;
                
            case do_replace:
                for( size_t i=0; i < size; ++i )
                {
                    indx[i] = g.full<size_t>() % size;
                }
                break;
        };
    }
    
}

