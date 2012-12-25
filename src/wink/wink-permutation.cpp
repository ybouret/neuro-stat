#include "wink-permutation.hpp"
#include <iostream>

namespace wink
{
    
    std::ostream & operator<<( std::ostream &os, const permutation &perm)
    {
        os << "[";
        for(size_t i=0;i<perm.size;++i)
            os << " " << perm.indx[i];
        os << " ]";
        return os;
    }

    permutation:: ~permutation() throw()
    {
        delete []indx;
    }
    
    permutation:: permutation( size_t n ) :
    size(n),
    indx(0),
    mode( do_shuffle )
    {
        if( size < 1 ) throw "invalid permutation size";
        indx = new size_t[ size ];
        identity();
    }
    
    void permutation::identity() throw()
    {
        assert(indx!=0);
        assert(size>0);
        for( size_t i=0; i < size; ++i ) indx[i] = i;
    }
    
    void permutation::generate( urand32 &g ) throw()
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

