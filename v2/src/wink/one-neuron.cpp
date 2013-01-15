#include "./one-neuron.hpp"
#include "../pyck/exception.hpp"
#include <new>

namespace wink
{
    one_neuron:: ~one_neuron() throw()
    {
	delete [] workspace;
    }
    
    one_neuron:: one_neuron( size_t num_trials, size_t max_data ) :
    records( num_trials  ),
    length( max_data ),
    stride( length+1 ),
    workspace( new double [stride * num_trials] ),
    trials( records.size )
    {
        double *p = workspace;
        for(size_t i=0; i < trials; ++i, p+=stride )
        {
            p[0] = double( length );
            new (&records[i]) record(p);
        }
    }
    
    const record & one_neuron:: operator[]( size_t indx ) const throw()
    {
        assert(indx<trials);
        return records[indx];
    }
    
    record & one_neuron:: operator[]( size_t indx )  throw()
    {
        assert(indx<trials);
        return records[indx];
    }
    
        
    void one_neuron:: loadR( const double *Rmat, size_t nrow, size_t ncol )
    {
        // sanity check
        if(!Rmat)        throw Exception("experiment: NULL R matrix");
        if(nrow!=trials) throw Exception("experiment: R rows!= trials");
        if(ncol>length)  throw Exception("experiment: R ncol>length");
        
        // load data
        double *p = workspace;
        for(size_t i=0; i < trials; ++i, p += stride )
        {
            assert( length == size_t(p[0]));
            for( size_t j=0; j < ncol; ++j )
            {
                p[j+1] = Rmat[i+j*nrow];
            }
        }
        
    }

    void one_neuron:: prepare_windows( double a, double b ) throw()
    {
        for( size_t i=0; i < trials; ++i )
            records[i].prepare(a,b);
    }
    
    
}
