#ifndef WINK_EXPERIMENT_INCLUDED
#define WINK_EXPERIMENT_INCLUDED 1

#include "./record.hpp"
#include "../pyck/c_array.hpp"

namespace wink
{
    //! experiment: content #trials records
    class experiment
    {
    public:
        explicit experiment( size_t num_trials, size_t max_data );
        virtual ~experiment() throw();
        
        
        const record & operator[]( size_t indx ) const throw();
        record       & operator[]( size_t indx ) throw();
        
        
        //! load a column-major R matrix
        /**
         \param Rmat the data in column-major layout
         \param nrow, must be equal to trials
         \param ncolm must be lower than records size.
         */
        void loadR( const double *Rmat, size_t nrow, size_t ncol );
        
    private:
        C_Array<record> records;
        const size_t    length;      //!< max_data
        const size_t    stride;
        double         *workspace;   //!< (max_data+1) * num_trials
        
        experiment(const experiment &);
        experiment&operator=(const experiment &);
        
    public:
        const size_t &trials;
    };
}


#endif
