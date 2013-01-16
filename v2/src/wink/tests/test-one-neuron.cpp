#include "../neuron.hpp"
#include "../../pyck/rand32.hpp"
#include "../../pyck/wtime.hpp"

#include <iostream>

using namespace wink;

int main(int argc, char *argv[] )
{
    try
    {
        DefaultUniformGenerator ran;
        ran.seed( WallTime::Seed() );
        
        neuron xp(5,100);
        std::cerr << "Creating " << xp.trials << " trials..." << std::endl;
        for( size_t i=0; i < xp.trials; ++i )
        {
            record &rec = xp[i];
            std::cerr << "rec.size=" << rec.size << std::endl;
            ran.fill_array(0, 10, &rec[1], rec.size);
        }
        
        const double delta = 0.2;
        cFile fp("conc0.dat", cFile::Overwrite);
        for( double a=0;a<10; a += 0.1 )
        {
            fp("%g",a);
            const double b = a + 1;
            xp.prepare_windows(a,b);
            for( size_t i=0; i < xp.trials; ++i )
            {
                for( size_t j=i; j < xp.trials; ++j )
                {
                    const size_t nc = xp[i].coincidences_with( xp[j], delta );
                    fp(" %u", unsigned(nc) );
                }
            }
            fp("\n");
        }
        
        
        return 0;
    }
    catch( const std::exception &e )
    {
        std::cerr << "*** Exception: " << e.what() << std::endl;
    }
    catch(...)
    {
        std::cerr << "Unhandled Exception!" << std::endl;
    }
    return 1;
}
