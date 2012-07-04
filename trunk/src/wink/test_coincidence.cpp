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
        const size_t Nx = 10 + size_t( wink::alea() * 20 );
        const size_t Ny = 10 + size_t( wink::alea() * 20 );
        
        double *M = new double [Nx+Ny];
        double *X = M;
        double *Y = M + Nx;
        
        fill_array(X,Nx); save_array(X, Nx, "x.dat");
        fill_array(Y,Ny); save_array(Y, Ny, "y.dat");
        const size_t nc = wink::coincidences(X, Nx, Y, Ny, 0.01);
        std::cerr << "Nx=" << Nx << std::endl;
        std::cerr << "Ny=" << Ny << std::endl;
        std::cerr << "NC=" << nc << std::endl;
        
        return 0;
    }
    catch(...)
    {
        std::cerr << "Error detected" << std::endl;
    }
    return 1;
}