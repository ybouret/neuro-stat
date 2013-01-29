#include "../neurons.hpp"
#include "../../pyck/wtime.hpp"
#include "../../pyck/stats.hpp"

#include <iostream>

using namespace wink;

#define __CHRONO(ELL,CODE) do { const double tmx = chrono.query(); CODE; ELL += chrono.query() - tmx; } while(false)

int main(int argc, char *argv[] )
{
    try
    {
        WallTime     chrono;
        neurons      xp;
        const size_t trials = 10;
        neuron       N1(trials,100 + xp.ran.leq(20));
        neuron       N2(trials,100 + xp.ran.leq(20));
        
        for(size_t i=0; i < trials; ++i )
        {
            xp.ran.fill_array(0, 10, &N1[i][1], N1[i].size());
            xp.ran.fill_array(0, 10, &N2[i][1], N2[i].size());
        }
        
        const double     delta = 0.2;

        for( double a=0;a<10; a += 0.1 )
        {
            const double b = a + 1;
            count_t T0 = xp.true_coincidences( statistic_T, N1, N2, a, b, delta);
            count_t H0 = xp.true_coincidences( statistic_H, N1, N2, a, b, delta);
            
            count_t T1 = xp.self_coincidences( statistic_T, N1, N2, a, b, delta);
            count_t H1 = xp.self_coincidences( statistic_H, N1, N2, a, b, delta);
            
            std::cerr << "T: " << T0 << "/" << T1 << " | H: " << H0 << "/" << H1 << std::endl;
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

