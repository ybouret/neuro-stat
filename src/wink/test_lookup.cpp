#include "wink.hpp"
#include <iostream>

static inline
void fill_array( double *x, size_t n )
{
    for( size_t i=0; i < n; ++i ) x[i] = wink::alea();
    wink::sort_array(x,n);
}

static inline void save_array( double *x, size_t n, const char *filename )
{
    FILE *fp = fopen(filename,"wb");
    if(!fp)
    {
        std::cerr << "Can't open " << filename << std::endl;
        throw;
    }
    
    for( size_t i=0; i < n; ++i )
    {
        fprintf(fp, "%.15g\n", x[i]);
    }
    fclose(fp);
}

int main( int argc, char *argv[] )
{
    
    try
    {
        wink::init_alea();
        for( size_t iter=1; iter <= 1024; ++iter )
        {
            const size_t Nx = 10 + size_t( wink::alea() * 100.0 );
            std::cerr << "Nx=" << Nx << std::endl;
            
            double *X = new double[Nx+1];
            X[0] = Nx;
            fill_array(X+1,Nx); save_array(X+1, Nx, "x1.dat");
            
            FILE *fp = fopen("win.dat","wb");
            if(!fp)
            {
                std::cerr << "Can't open file" << std::endl;
                throw;
            }
            
            const double step = 0.01;
            for( double a = -5*step; a <= 1+5*step; a += step )
            {
                size_t ia=0;
                size_t ib=0;
                const bool found_a = wink::find_index_after(a, X, ia);
                const bool found_b = wink::find_index_before(a, X, ib);
                fprintf(fp,"%g", a);
                if( found_a ) fprintf(fp," %u", unsigned(ia) ); else fprintf(fp," -1");
                if( found_b ) fprintf(fp," %u", unsigned(ib) ); else fprintf(fp," -1");
                fprintf(fp, "\n");
                
            }
            fclose(fp);
        }
        return 0;
    }
    catch(...)
    {
        std::cerr << "Error detected" << std::endl;
    }
    return 1;
}