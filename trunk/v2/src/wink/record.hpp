#ifndef WINK_RECORD_INCLUDED
#define WINK_RECORD_INCLUDED 1

#include "../pyck/os.hpp"
#include "./iwindow.hpp"

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
        explicit     record( double *data ) throw();
        
        const double &operator[]( size_t indx ) const throw();
        double       &operator[]( size_t indx ) throw();
        
        
        const size_t size; //!< keep a copy of data[0]
        iwindow      iwin; //!< for computation
        void         prepare(double a,double b) throw();
        
        //! count coincidences between two records and their prepared windows
        size_t coincidences_with( const record &other, const double delta ) const throw();
        
    private:
        double *addr; //!< X[1..n]
        record( const record & );
        record&operator=(const record&);
    };
    
}

#endif
