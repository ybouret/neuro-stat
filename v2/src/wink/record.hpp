#ifndef WINK_RECORD_INCLUDED
#define WINK_RECORD_INCLUDED 1

#include "../pyck/os.hpp"

namespace wink
{
    //! one neuronal record, R kind
	class record
    {
    public:
        //! build from data
        /**
         \param   data data[0] = n, data[1..n]=tops
         \warning data must be available, no memory is allocated !
         */
        explicit record( double *data ) throw();
        const size_t size;
        
        inline const double &operator[]( size_t indx ) const throw();
        inline double       &operator[]( size_t indx ) throw();
        
    private:
        double *addr; //!< X[1..n]
        record( const record & );
        record&operator=(const record&);
    };
    
}

#endif

