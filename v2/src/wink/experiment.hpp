#ifndef WINK_EXPERIMENT_INCLUDED
#define WINK_EXPERIMENT_INCLUDED 1

#include "./record.hpp"
#include "../pyck/c_array.hpp"

namespace wink
{
    class experiment
    {
    public:
        explicit experiment( size_t num_trials, size_t max_data );
        virtual ~experiment() throw();
        
        size_t trials() const throw();
        
        const record & operator[]( size_t indx ) const throw();
        
        
    private:
        C_Array<record> records;
        const size_t    length;      //!< max_data
        const size_t    stride;
        double         *workspace;   //!< (max_data+1) * num_trials
        
        experiment(const experiment &);
        experiment&operator=(const experiment &);

    };
}


#endif
