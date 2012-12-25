#define _CRT_SECURE_NO_WARNINGS
#include "../wink-permutation.hpp"
#include <iostream>
#include <cstdio>
#include <ctime>
#include <cmath>

static inline void ave_and_sig( double &ave, double &sig, const double *v, size_t n )
{
    ave = 0;
    for( size_t i=0; i < n; ++i ) ave += v[i];
    ave /= n;
    
    sig = 0;
    for( size_t i=0; i<n; ++i )
    {
        const double tmp = ave - v[i];
        sig += tmp * tmp;
    }
    sig = sqrt(sig/(n-1));
    
}

static void perform(wink::urand32               &g,
                    wink::permutation::mode_type m,
                    double      *v1,
                    double      *v2,
                    double      *v3,
                    size_t       COUNT,
                    const double AVE,
                    const double SIG,
                    const char   *fn)
{
    assert(fn);
    std::cerr << "Creating " << fn << std::endl;
    FILE *fp = fopen(fn,"wb");
    if( !fp )
    {
        std::cerr << "Can't open file" << std::endl;
        throw;
    }
    
    
    for( size_t iter = 1; iter <= 64; ++iter )
    {
        const size_t      n = 10 + g.less_than(1000);
        //std::cerr << "perm.size=" << n << std::endl;
        wink::permutation perm( n );
        perm.mode = m;
        const size_t idx1=0;
        const size_t idx2=n/2;
        const size_t idx3=n-1;
        for(size_t count=0; count < COUNT; ++count )
        {
            perm.generate(g);
            v1[count] = perm.indx[idx1];
            v2[count] = perm.indx[idx2];
            v3[count] = perm.indx[idx3];
        }
        
        double ave1,sig1;
        double ave2,sig2;
        double ave3,sig3;
        ave_and_sig(ave1, sig1, v1, COUNT);
        ave_and_sig(ave2, sig2, v2, COUNT);
        ave_and_sig(ave3, sig3, v3, COUNT);
        
        ave1 /= n-1;
        ave2 /= n-1;
        ave3 /= n-1;
        sig1 /= n-1;
        sig2 /= n-1;
        sig3 /= n-1;
        
        fprintf(fp,"%u %.4f %.4f %.4f %.4f %.4f %.4f\n", unsigned(iter), ave1/AVE, ave2/AVE, ave3/AVE, sig1/SIG, sig2/SIG, sig3/SIG );
        fprintf(stderr,".");
        if( 0 == (iter&31) )
            fprintf(stderr,"\n");
        fflush(stderr);
    }
    fclose(fp);
    fprintf(stderr,"\n");
    
}

int main( int argc, char *argv[] )
{
    
    try
    {
        static const size_t COUNT = 1000;
        double             *v1    = new double[ COUNT * 3];
        double             *v2    = v1 + COUNT;
        double             *v3    = v2 + COUNT;
        std::cerr << "-- Generate " << COUNT << " permutations/iteration" << std::endl;
        std::cerr << "-- Init random" << std::endl;
        
        wink::rand32_kiss g;
        g.seed( uint32_t(time(NULL)) );
        
        std::cerr << "-- Testing drawings" << std::endl;
        {
            wink::permutation perm(10);
            perm.identity();
            std::cerr << "id=" << perm << std::endl;
            
            std::cerr << std::endl;
            for( size_t j=0; j < 4; ++j )
            {
                perm.mode = wink::permutation::do_shuffle;
                perm.generate(g);
                std::cerr << "sh=" << perm << std::endl;
            }
            
            std::cerr << std::endl;
            for( size_t j=0; j < 4; ++j )
            {
                perm.mode = wink::permutation::do_replace;
                perm.generate(g);
                std::cerr << "rp=" << perm << std::endl;
            }

            
        }
        return 0;
        
        std::cerr << "-- Testing properties" << std::endl;
        double AVE = 0.5;
        double SIG = 1.0/sqrt(12.0);
        perform(g, wink::permutation::do_shuffle, v1, v2, v3, COUNT, AVE, SIG, "perm-shuffle.dat");
        
        AVE = 1;
        SIG = 1;
        perform(g, wink::permutation::do_replace, v1, v2, v3, COUNT, AVE, SIG, "perm-replace.dat");
        
        
        fprintf(stderr,"\n");
        
        delete  []v1;
        return 0;
    }
    catch(...)
    {
        std::cerr << "Error detected" << std::endl;
    }
    return 1;
}
