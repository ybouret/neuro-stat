//! \file
/**
 \brief memory and algorithm for permutation
 */
#ifndef WINK_PERMUTATION_INCLUDED
#define WINK_PERMUTATION_INCLUDED 1


#include "wink-rand32.hpp"

namespace wink 
{
    
    //! holds memory for a permutation
    class permutation
    {
    public:
        permutation( size_t n );
        ~permutation() throw();
        
        const size_t size;
        size_t      *indx;
        
        //! build the identity permutation
        void identity() throw();
        
        //! build a random permutation
        void generate( urand32 &g ) throw();
        
    private:
        permutation( const permutation & );
        permutation&operator=(const permutation & );
    };

    
}

#endif
