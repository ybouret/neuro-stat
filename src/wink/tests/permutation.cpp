#include "../wink-permutation.hpp"
#include <iostream>
#include <cstdio>
#include <time.h>

int main( int argc, char *argv[] )
{
    
    try
    {
        static const size_t COUNT = 2048;
        wink::rand32_kiss g;
        g.seed( time(NULL) );
        FILE *fp = fopen("perm.dat","wb");
        if( !fp )
        {
            std::cerr << "Can't open file" << std::endl;
            throw;
        }
        
        for( size_t iter = 1; iter <= 32; ++iter )
        {
            const size_t      n = 10 + g.less_than(1000);
            std::cerr << "perm.size=" << n << std::endl;
            wink::permutation perm( n );
            double ave1 = 0;
            double ave2 = 0;
            double ave3 = 0;
            const size_t idx1=0;
            const size_t idx2=n/2;
            const size_t idx3=n-1;
            for(size_t count=1; count <= COUNT; ++count )
            {
                perm.generate(g);
#if 0
                for( size_t i=0; i < n; ++i )
                    std::cerr << " " << indices[i];
                std::cerr << std::endl;
#endif
                ave1 += perm.indx[idx1];
                ave2 += perm.indx[idx2];
                ave3 += perm.indx[idx3];
            }
            ave1 /= COUNT;
            ave1 /= n-1;
            ave2 /= COUNT;
            ave2 /= n-1;
            ave3 /= COUNT;
            ave3 /= n-1;
            fprintf(fp,"%u %.4f %.4f %.4f\n", unsigned(iter), ave1, ave2, ave3 );
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
