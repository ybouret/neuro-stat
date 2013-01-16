#ifndef WINK_DRAWING_INCLUDED
#define WINK_DRAWING_INCLUDED 1


#include "../pyck/pair.hpp"

namespace wink
{
    typedef Pair<size_t,size_t> ipair;
    
    class couple
    {
    public:
        size_t first;
        size_t second;
        couple *next;
        couple *prev;
        
    private:
        couple(const couple &);
        couple&operator=(const couple &);
    };
    
    class drawing
    {
    public:
        explicit drawing( size_t n );
        virtual ~drawing() throw();
        
        const size_t trials;
        const size_t pairs;
        
    private:
        drawing(const drawing & );
        drawing&operator=(const drawing &);
        
        ipair *reg;
        
    };
}

#endif
