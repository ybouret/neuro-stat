#include "./drawing.hpp"
#include <cstring>
#include <iostream>
#include "../pyck/c_array.hpp"

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
    
    drawing:: drawing() throw() :
    couple::list(),
    pool()
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
            couple *cpl = new couple;
            memset(cpl,0,sizeof(couple));
            pool.store( cpl );
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
    
    void drawing:: permutation( size_t n, UniformGenerator &ran )
    {
        clear();
        if(n>0)
        {
            C_Array<double> arr(n);
            for(size_t i=0; i <n; ++i) arr[i] = i;
            ran.shuffle(&arr[0],n);
            for( size_t i=0; i < n; ++i )
            {
                couple *cpl = query();
                cpl->first  = i;
                cpl->second = arr[i];
                push_back(cpl);
            }
        }
        assert(size==n);
        
    }
    
    void drawing:: replacement( size_t n, UniformGenerator &ran )
    {
        clear();
        for( size_t i=0; i < n; ++i)
        {
            couple *cpl = query();
            cpl->first  = i;
            cpl->second = ran.lt(n);
            push_back(cpl);
        }
        assert(size==n);
    }

    
    std::ostream & operator<<( std::ostream &os, const drawing &d)
    {
        os << "{";
        for( const couple *c = d.head; c; c=c->next )
        {
            os << " (" << c->first << "," << c->second << ")";
        }
        os << " }";
        return os;
    }
    
}
