/**
 \file
 \brief Memory and Algorithm for permutation
 */
#ifndef WINK_PERMUTATION_INCLUDED
#define WINK_PERMUTATION_INCLUDED 1


#include "wink-rand32.hpp"
#include <iosfwd>

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
        
        //! build a drawing according to mode
        void generate( urand32 &g ) throw();
        
        friend std::ostream & operator<<( std::ostream &, const permutation & );
        
    private:
        permutation( const permutation & );
        permutation&operator=(const permutation & );
    };

    
}

#endif
