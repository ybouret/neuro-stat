#include "../wink.hpp"
#include <iostream>

int main( int argc, char *argv[] )
{
    
    wink::init_alea();
    try
    {
        const double L = 10.0;
        for( size_t iter = 1; iter <= 1; ++iter )
        {
            const size_t ntrials = 5  + size_t( 10 * wink::alea() );
            const size_t ntop    = 10 + size_t( 10 * wink::alea() );
            const size_t nr      = ntrials * 2;
            const size_t nc      = ntop + 1;
            const size_t nd      = nr * nc;
            double      *data    = new double[nd];
            for( size_t i=0; i < nr; ++i )
            {
                double      *X  = data + (i*nc);
                const size_t Nx = wink::random_index(wink::alea) % (ntop+1);
                X[0] = Nx;
                wink::fill_alea_array(X+1, Nx, 0, L);
            }
            
            
            wink::neuro_trials N1(data,     ntrials,nc);
            wink::neuro_trials N2(data+nd/2,ntrials,nc);
            
            
            std::cerr << "N1: " << std::endl;
            N1.display();
            std::cerr << "N2: " << std::endl;
            N2.display();
            
            
            delete []data;
        }
    
        return 0;
    }
    catch(...)
    {
        std::cerr << "Error detected" << std::endl;
    }
    return 1;
}
