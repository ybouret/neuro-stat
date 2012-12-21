/**
 \file
 \brief C-style matrices
 */

#ifndef WINK_C_MATRIX_INCLUDED
#define WINK_C_MATRIX_INCLUDED 1


#include <stddef.h>

namespace wink 
{
    //! used to convert R matrix into C matrix
    class c_matrix
    {
    public:
        c_matrix( size_t nr, size_t ncol );
        ~c_matrix() throw();
        const size_t nrow; //! #rows
        const size_t ncol; //! #columns
        double      *data; //! row-major data[ nrow*ncol ]
        
        //! load a column major R matrix
        void loadR( const double *Rdata ) throw();
        
    private:
        c_matrix(const c_matrix &);
        c_matrix&operator=(const c_matrix & );
    };
}

#endif
