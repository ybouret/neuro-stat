#include "../wink.hpp"
#include <iostream>



int main( int argc, char *argv[] )
{
    
    try
    {
        wink::init_alea();
        const double L = 10.0;
        const double Lo = -1;
        const double Hi = L+Lo;
        const double Width = Hi-Lo;
        for( size_t iter=1; iter <= 1024; ++iter )
        {
            const size_t Nx = 10 + size_t( wink::alea() * 500.0 );            
            double *X = new double[Nx+1];
            X[0] = Nx;
            wink::fill_alea_array(X+1,Nx,0,L); 
                        
            for( size_t num=1; num <= 1024; ++num )
            {
                const double a = Lo + Width * wink::alea();
                const double b = a  + Width * wink::alea();
                wink::iwindow w;
                w.initialize(a, b, X);
            }
            
            delete []X;
        }
        return 0;
    }
    catch(...)
    {
        std::cerr << "Error detected" << std::endl;
    }
    return 1;
}
