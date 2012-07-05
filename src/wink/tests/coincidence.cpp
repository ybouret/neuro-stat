#include "../wink-rand32.hpp"
#include "../wink-coincidence.hpp"

#include <iostream>
#include <cstdio>



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
        wink::rand32_kiss g;
        g.seed( time(NULL) );
        const size_t Nx = 10 + g.less_than(20);
        const size_t Ny = 10 + g.less_than(20);
        
        double *M = new double [Nx+Ny];
        double *X = M;
        double *Y = M + Nx;
        
        g.fill_array(0,1,X,Nx); save_array(X, Nx, "x.dat");
        g.fill_array(0,1,Y,Ny); save_array(Y, Ny, "y.dat");
        const size_t nc = wink::coincidences(X, Nx, Y, Ny, 0.01);
        std::cerr << "NC=" << nc << std::endl;
        
        delete []M;
        return 0;
    }
    catch(...)
    {
        std::cerr << "Error detected" << std::endl;
    }
    return 1;
}
