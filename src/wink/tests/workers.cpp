#include "../par/wink-worker.hpp"
#include <iostream>
#include <cstdio>
#include <ctime>
#include <cstdlib>

static const double L = 10.0;

static inline
void fill_matrix( wink::c_matrix &M, wink::urand32 &g )
{
    const size_t ntop = M.ncol-1;
    for( size_t i=0; i < M.nrow; ++i )
    {
        double      *X  = M.data + (i*M.ncol);
        const size_t Nx = g.less_than(ntop);
        X[0] = Nx;
        g.fill_array(0, L, X+1, Nx);
    }
}

int main( int argc, char *argv[] )
{
    size_t num_threads = 1;
    if( argc >= 2 )
    {
        num_threads = atol( argv[1] );
        if( num_threads <= 0 )
            num_threads = 1;
    }
    
    try
    {
        wink::rand32_kiss g;
        g.seed( uint32_t(time(NULL)) );
        
        
        const size_t ntrials = 5  + g.less_than(30);
        
        wink::c_matrix M1(ntrials,5+g.less_than(200));
        wink::c_matrix M2(ntrials,5+g.less_than(200));
        fill_matrix(M1,g);
        fill_matrix(M2,g);
        
        const size_t   B     = 1000;
        const double   delta = 0.005;
        
        const size_t   num_windows = 500 + g.less_than(500);
        double        *pvalues     = new double[3*num_windows];
        double        *windows     = pvalues + num_windows;
        
        std::cerr << "num windows=" << num_windows << std::endl;
        
        const double fac = 1.0 / (num_windows);
        for( size_t i=0; i < num_windows; ++i )
        {
            double &a = windows[ 0 + i*2 ];
            double &b = windows[ 1 + i*2 ];
            a = (i*L) *fac;
            b = ((i+1)*L)*fac;
        }
        
        try
        {
            wink::workers team(M1,M2,B,num_threads,num_windows,windows,pvalues,delta);
            
            team.wait();
            
        }
        catch(...)
        {
            delete []pvalues;
            throw;
        }
        
        
        delete [] pvalues;
        return 0;
    }
    catch(...)
    {
        std::cerr << "Error in program" << std::endl;
    }
    return 1;
}
