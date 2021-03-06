/**
 \file
 \brief overlay a record structure on a matrix
 */
#ifndef WINK_RECORD_INCLUDED
#define WINK_RECORD_INCLUDED 1

#include "../pyck/os.hpp"
#include "./iwindow.hpp"
#include "./types.hpp"

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
        
        //! return data[1<=indx<=size()]
        const double &operator[]( size_t indx ) const throw();
        
        //! return data[1<=indx<=size()]
        double       &operator[]( size_t indx ) throw();
        
        //! data[0]
        size_t size() const throw();
        
        iwindow      iwin; //!< for computation
        void         prepare(double a,double b) throw(); //!< prepare the window
        
        //! count coincidences between two records and their prepared windows
        count_t coincidences_with( const record &other, const double delta ) const throw();
        
    private:
        double *addr; //!< addr[1..n] are the tops value
        record( const record & );
        record&operator=(const record&);
    };
    
}

#endif

