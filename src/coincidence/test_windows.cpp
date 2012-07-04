#include "coincidence.hpp"
#include <iostream>

static inline
void fill_array( double *x, size_t n )
{
    for( size_t i=0; i < n; ++i ) x[i] = alea();
    sort_array(x,n);
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
        init_alea();
        const size_t Nx = 10 + size_t( alea() * 100.0 );
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
        for( double a = -step; a <= 1+step; a += step )
        {
            size_t i=0;
            if( ! find_index_greater_than(a, X, i) )
            {
                fprintf(fp,"%g -1\n",a);
            }
            else
            {
                fprintf(fp,"%g %u\n", a, unsigned(i) );
            }
        }
        fclose(fp);
        return 0;
    }
    catch(...)
    {
        std::cerr << "Error detected" << std::endl;
    }
    return 1;
}