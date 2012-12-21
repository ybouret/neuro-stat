/**
 \file
 \brief Memory and Algorithm for permutation
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
        enum mode_type
        {
            do_shuffle,
            do_replace
        };
        
        permutation( size_t n );
        ~permutation() throw();
        
        const size_t size; //!< #indices
        size_t      *indx; //!< the indices
        mode_type    mode; //!< default: do_shuffle
        
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
