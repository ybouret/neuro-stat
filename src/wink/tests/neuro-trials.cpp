#include "../wink.hpp"
#include <iostream>
#include <cstdio>
#include <time.h>


int main( int argc, char *argv[] )
{
    
    try
    {
        
        wink::rand32_kiss g;
        g.seed( time(NULL) );
        const size_t Bcount = 500;
        size_t      *Bcoinc = new size_t[ Bcount ];
        
        const double L = 10.0;
        for( size_t iter = 1; iter <= 1; ++iter )
        {
            const size_t ntrials = 5  + g.less_than(30);
            const size_t ntop    = 50 + g.less_than(100);
            const size_t nr      = ntrials * 2;
            const size_t nc      = ntop + 1;
            const size_t nd      = nr * nc;
            double      *data    = new double[nd];
            for( size_t i=0; i < nr; ++i )
            {
                double      *X  = data + (i*nc);
                const size_t Nx = g.less_than(ntop); //0--ntop
                assert(Nx<=ntop);
                X[0] = Nx;
                g.fill_array(0,L,X+1, Nx);
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
                const double b = a + w0 + w0 * g.alea();
                N1.prepare_windows(a, b);
                N2.prepare_windows(a, b);
                
                const size_t true_coinc = wink::true_coincidences(N1, N2,delta,perm);
                std::cerr << "true_coinc [" << a << "," << b <<"]=" << true_coinc << std::endl;
                wink::permutation_bootstrap(Bcoinc, Bcount, N1, N2, delta, perm, g );
                const double pvalue = wink::permutation_pvalue(true_coinc, Bcoinc, Bcount);
                std::cerr << "pvalue=" << pvalue << std::endl;
                fprintf(fp, "%g %g %g\n", a,b,pvalue);
                
            }
            fclose(fp);
            
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
