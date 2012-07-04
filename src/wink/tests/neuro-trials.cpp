#include "../wink.hpp"
#include <iostream>

int main( int argc, char *argv[] )
{
    
    wink::init_alea();
    try
    {
        const size_t Bcount = 500;
        size_t      *Bcoinc = new size_t[ Bcount ];
        
        const double L = 10.0;
        for( size_t iter = 1; iter <= 1; ++iter )
        {
            const size_t ntrials = 5  + size_t( 10 * wink::alea() );
            const size_t ntop    = 50 + size_t( 100 * wink::alea() );
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
            
            wink::permutation perm(ntrials);
            
            const double step  = 0.05;
            const double w0    = 0.5;
            const double delta = 0.1;
            FILE *fp = fopen("pvalue.dat","wb");
            if(!fp)
                throw "can't open pvalue.dat";
            
            for( double a = -step; a <= L + step; a += step )
            {
                const double b = a + w0 + w0 * wink::alea();
                N1.prepare_windows(a, b);
                N2.prepare_windows(a, b);
                
                const size_t true_coinc = wink::true_coincidences(N1, N2,delta,perm);
                //std::cerr << "true_coinc [" << a << "," << b <<"]=" << true_coinc << std::endl;
                wink::permutation_bootstrap(Bcoinc, Bcount, N1, N2, delta, perm, wink::alea );
                const double pvalue = wink::permutation_pvalue(true_coinc, Bcoinc, Bcount);
                fprintf(fp, "%g %g %g\n", a,b,pvalue);
                
            }
            
            
            delete []data;
        }
    
        delete []Bcoinc;
        return 0;
    }
    catch(...)
    {
        std::cerr << "Error detected" << std::endl;
    }
    return 1;
}
