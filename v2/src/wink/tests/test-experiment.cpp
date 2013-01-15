#include "../experiment.hpp"
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
        
        experiment xp(5,100);
        for( size_t i=0; i < xp.trials; ++i )
        {
            record &rec = xp[i];
            std::cerr << "rec.size=" << rec.size << std::endl;
            ran.fill_array(0, 10, &rec[1], rec.size);
        }
        
        const double delta = 0.2;
        for( double a=0;a<10; a += 0.1 )
        {
            const double b = a + 1;
            xp.prepare_windows(a,b);
            for( size_t i=0; i < xp.trials; ++i )
            {
                for( size_t j=i; j < xp.trials; ++j )
                {
                    const size_t nc = xp[i].coincidences_with( xp[j], delta );
                }
            }
        }
        
        return 0;
    }
    catch(...)
    {
        std::cerr << "Unhandled Exception!" << std::endl;
    }
    return 1;
}