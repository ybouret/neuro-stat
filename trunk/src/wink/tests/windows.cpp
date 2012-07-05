#include "../wink-iwindow.hpp"
#include "../wink-rand32.hpp"
#include <iostream>
#include <time.h>


int main( int argc, char *argv[] )
{
    
    try
    {
        wink::rand32_kiss g;
        g.seed( time(NULL) );
        const double L = 10.0;
        const double Lo = -1;
        const double Hi = L+Lo;
        const double Width = Hi-Lo;
        for( size_t iter=1; iter <= 1024; ++iter )
        {
            const size_t Nx = 10 + g.less_than(500);           
            double *X = new double[Nx+1];
            X[0] = Nx;
            g.fill_array(0,L,X+1,Nx); 
                        
            for( size_t num=1; num <= 1024; ++num )
            {
                const double a = Lo + Width * g.alea();
                const double b = a  + Width * g.alea();
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
