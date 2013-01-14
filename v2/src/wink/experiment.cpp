#include "./experiment.hpp"
#include "../pyck/exception.hpp"
#include <new>

namespace wink
{
    experiment:: ~experiment() throw()
    {
    }
    
    experiment:: experiment( size_t num_trials, size_t max_data ) :
    records( num_trials  ),
    length( max_data ),
    stride( length+1 ),
    workspace( new double [stride * num_trials] )
    {
        double *p = workspace;
        for(size_t i=0; i < trials(); ++i, p+=stride )
        {
            p[0] = double( length );
            new (&records[i]) record(p);
        }
    }
    
    const record & experiment:: operator[]( size_t indx ) const throw()
    {
        assert(indx<trials());
        return records[indx];
    }
    
    size_t experiment:: trials() const throw()
    {
        return records.size;
    }
    
    
    
    
}
