#include "./record.hpp"
#include "../pyck/exception.hpp"

namespace wink
{
       
    record::record( const double *data ) throw():
    size( data[0] ),
    addr( data    )
    {
    }
   
    /*
    size_t record:: size() const throw()
    {
        return size_t(addr[0]);
    }
    */
    
    double record:: operator[]( size_t indx ) const throw()
    {
        assert( indx > 0 );
        assert( indx <= size );
        return addr[indx];
    }
    
}
