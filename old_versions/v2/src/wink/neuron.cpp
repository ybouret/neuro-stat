#include "./neuron.hpp"
#include "../pyck/exception.hpp"
#include <new>

namespace wink
{
    neuron:: ~neuron() throw()
    {
        delete [] workspace;
    }
    
    neuron:: neuron( size_t num_trials, size_t max_data ) :
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
    
    const record & neuron:: operator[]( size_t indx ) const throw()
    {
        assert(indx<trials);
        return records[indx];
    }
    
    record & neuron:: operator[]( size_t indx )  throw()
    {
        assert(indx<trials);
        return records[indx];
    }
    
    
    void neuron:: loadR( const double *Rmat, size_t nrow, size_t ncol )
    {
        // sanity check
        if(!Rmat)         throw Exception("neuron: NULL R matrix");
        if(nrow!=trials)  throw Exception("neuron: R rows!= trials");
        if(ncol>length+1) throw Exception("neuron: R ncol>length");
        if(ncol<=0)       throw Exception("neuron: R ncol<1");
        
        // load data
        double *p = workspace;
        
        for(size_t i=0; i < trials; ++i, p += stride )
        {
            //assert( length == size_t(p[0]));
            for( size_t j=0; j < ncol; ++j )
            {
                p[j] = Rmat[i+j*nrow];
            }
        }
        
    }
    
    void neuron:: prepare_windows( double a, double b ) throw()
    {
        for( size_t i=0; i < trials; ++i )
            records[i].prepare(a,b);
    }
    
    
}
