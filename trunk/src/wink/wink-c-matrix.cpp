#include "wink-c-matrix.hpp"
#include <cassert>

namespace wink
{
    c_matrix:: ~c_matrix() throw()
    {
        delete [] data;
    }
    
    
    c_matrix:: c_matrix( size_t nr, size_t nc ) :
    nrow( nr ),
    ncol( nc ),
    data( 0  )
    {
        assert(nrow>0);
        assert(ncol>0);
        data = new double[ nr * nc ];
    }
    
    void c_matrix:: loadR( const double *Rdata ) throw()
    {
        assert(Rdata!=NULL);
        for( size_t i=0; i < nrow; ++i)
        {
            double *r_i = data + (i*ncol);
            for(size_t j=0; j < ncol; ++j )
            {
                r_i[j] = Rdata[ i + nrow * j ];
            }
        }
        
    }
}