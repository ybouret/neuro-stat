#ifndef WINK_DRAWING_INCLUDED
#define WINK_DRAWING_INCLUDED 1

#include "wink-rand32.hpp"

namespace wink 
{
	
    
    //! holds memory for a drawing
    class drawing
    {
    public:
        enum mode_type
        {
            do_shuffle,
            do_replace
        };
        
        drawing( size_t n );
        ~drawing() throw();
        
        const size_t size; //!< #indices
        size_t      *indx; //!< the indices
        mode_type    mode; //!< default: do_shuffle
        
        //! build the identity drawing
        void identity() throw();
        
        //! build a random drawing
        void generate( urand32 &g ) throw();
        
    private:
        drawing( const drawing & );
        drawing&operator=(const drawing & );
    };
	
    
}

#endif
