#ifndef WINK_EXPERIMENT_INCLUDED
#define WINK_EXPERIMENT_INCLUDED 1

#include "./record.hpp"
#include "../pyck/c_array.hpp"
#include "../pyck/file.hpp"

namespace wink
{
    //! content #trials records for ONE neuron
    class one_neuron
    {
    public:
        explicit one_neuron( size_t num_trials, size_t max_data );
        virtual ~one_neuron() throw();
        
        
        const record & operator[]( size_t indx ) const throw();
        record       & operator[]( size_t indx ) throw();
        
        
        //! load a column-major R matrix
        /**
         \param Rmat the data in column-major layout
         \param nrow, must be equal to trials
         \param ncolm must be lower than records size.
         */
        void loadR( const double *Rmat, size_t nrow, size_t ncol );
        
        //! prepare windows for all records
        void prepare_windows( double a, double b ) throw();
        
    private:
        C_Array<record> records;
        const size_t    length;      //!< max_data
        const size_t    stride;
        double         *workspace;   //!< (max_data+1) * num_trials
        
        one_neuron(const one_neuron &);
        one_neuron&operator=(const one_neuron &);
        
    public:
        const size_t trials;
    };
}


#endif
