#include "../wink.hpp"
#include <iostream>
#include <cstdio>


int main( int argc, char *argv[] )
{
    
    try
    {
        static const size_t COUNT = 2048;
        wink::init_alea();
        FILE *fp = fopen("perm.dat","wb");
        if( !fp )
        {
            std::cerr << "Can't open file" << std::endl;
            throw;
        }
        
        for( size_t iter = 1; iter <= 16; ++iter )
        {
            const size_t   n       = 10 + size_t( wink::alea() * 10 );
            size_t        *indices = new size_t[ n ];
            double ave1 = 0;
            double ave2 = 0;
            double ave3 = 0;
            const size_t idx1=0;
            const size_t idx2=n/2;
            const size_t idx3=n-1;
            for(size_t count=1; count <= COUNT; ++count )
            {
                wink::build_permutation(indices,n);
#if 0
                for( size_t i=0; i < n; ++i )
                    std::cerr << " " << indices[i];
                std::cerr << std::endl;
#endif
                ave1 += indices[idx1];
                ave2 += indices[idx2];
                ave3 += indices[idx3];
            }
            ave1 /= COUNT;
            ave1 /= n-1;
            ave2 /= COUNT;
            ave2 /= n-1;
            ave3 /= COUNT;
            ave3 /= n-1;
            fprintf(fp,"%u %.4f %.4f %.4f\n", unsigned(iter), ave1, ave2, ave3 );
            delete [] indices;
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
