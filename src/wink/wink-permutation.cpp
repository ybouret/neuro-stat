#include "wink-permutation.hpp"

namespace wink
{
    
    
    permutation:: ~permutation() throw()
    {
        delete []indx;
    }
    
    permutation:: permutation( size_t n ) :
    size(n),
    indx(0)
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
        identity();
        g.shuffle<size_t>(indx,size);
    }
    
}

