//! \file
/**
 \brief memory and algorithm for permutation
 */
#ifndef WINK_PERMUTATION_INCLUDED
#define WINK_PERMUTATION_INCLUDED 1


#include "wink-rand32.hpp"

namespace wink 
{
#if 0
    //! build a random permutation
    /**
     \param indices will contained the permutted indices
     \param num indices[0..num]
     \param uniform_generator in [0:1]
     */
    void build_permutation( size_t *indices, size_t num, double (*uniform_generator)() );
    
    //! build an identity vector
    void build_identity( size_t *indices, size_t num );
#endif
    
    //! holds memory for a permutation
    class permutation
    {
    public:
        permutation( size_t n );
        ~permutation() throw();
        
        const size_t size;
        size_t      *indx;
        
        void identity() throw();
        void generate( urand32 &g ) throw();
        
    private:
        permutation( const permutation & );
        permutation&operator=(const permutation & );
    };

    
}

#endif
