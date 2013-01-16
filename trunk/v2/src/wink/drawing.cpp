#include "./drawing.hpp"
#include <cstring>

namespace wink
{

    void couple::kill( couple *c ) throw()
    {
        assert(c);
        delete c;
    }

    drawing:: ~drawing() throw()
    {
        couple::list &self = *this;
        pool.delete_with( couple::kill );
        self.delete_with( couple::kill );
    }
    
    drawing:: drawing() throw() : couple::list()
    {
        
    }
 
    void drawing:: clear() throw()
    {
        while( size ) pool.store( pop_back() );
        assert( 0 == size );
    }

    
    couple *drawing:: query()
    {
        couple *cpl = pool.size ? pool.query() : new couple;
        memset(cpl,0,sizeof(couple));
        return cpl;
    }
    
    void drawing:: reserve( size_t n )
    {
        for(size_t i=0; i<n; ++i)
        {
            pool.store( new couple );
        }
    }
    
    void drawing:: identity( size_t n )
    {
        clear();
        for( size_t i=0; i < n; ++i )
        {
            couple *cpl = query();
            cpl->first = cpl->second = i;
            push_back(cpl);
        }
        assert(n==size);
    }

    
}
