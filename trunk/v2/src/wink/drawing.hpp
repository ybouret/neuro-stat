#ifndef WINK_DRAWING_INCLUDED
#define WINK_DRAWING_INCLUDED 1


#include "../pyck/pool.hpp"
#include "../pyck/list.hpp"
#include "../pyck/rand32.hpp"
#include <iosfwd>

namespace wink
{
    
    struct couple
    {
        size_t  first;
        size_t  second;
        couple *next;
        couple *prev;
        
        typedef ListOf<couple> list;
        typedef PoolOf<couple> pool;
        static void kill( couple * ) throw();
    };
    
    class drawing : public couple::list
    {
    public:
        explicit drawing() throw();
        virtual ~drawing() throw();
        
        void clear() throw();
        
        void reserve( size_t n );
        void identity( size_t n );
        void permutation( size_t n, UniformGenerator &ran );
        
        friend std::ostream & operator<<( std::ostream &, const drawing & );
        
    private:
        couple *query();
        couple::pool pool;
        drawing(const drawing &);
        drawing&operator=(const drawing &);
    };
    
}

#endif
