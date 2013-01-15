#include "./record.hpp"
#include "../pyck/exception.hpp"

namespace wink
{
    
    record::record( double *data ) throw():
    size( data[0] ),
    addr( data    )
    {
    }
    
    
    
    const double &record:: operator[]( size_t indx ) const throw()
    {
        assert( indx > 0 );
        assert( indx <= size );
        return addr[indx];
    }
    
    double & record:: operator[]( size_t indx )  throw()
    {
        assert( indx > 0 );
        assert( indx <= size );
        return addr[indx];
    }
    
    void record:: prepare( double a, double b ) throw()
    {
        iwin.initialize(a, b, addr);
    }
    
    size_t record:: coincidences_with( const record &other, const double delta ) const throw()
    {
        return count_coincidences(addr,
                                  iwin,
                                  other.addr,
                                  other.iwin,
                                  delta);
    }
    
    
}
