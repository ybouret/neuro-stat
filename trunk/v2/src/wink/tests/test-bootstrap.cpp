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
            xp.ran.fill_array(0, 10, &N1[i][1], N1[i].size);
            xp.ran.fill_array(0, 10, &N2[i][1], N2[i].size);
        }
        
        C_Array<size_t> Bcoinc(8192);
        const size_t    nb    = Bcoinc.size;
        const double    delta = 0.2;
        cFile fp("coinc1.dat", cFile::Overwrite);
        chrono.start();
        double ell_perm = 0;
        double ell_repl = 0;
        size_t count    = 0;
        double ave      = 0;
        double sig      = 0;
        for( double a=0;a<10; a += 0.1 )
        {
            ++count;
            fp("%g",a);
            const double b          = a + 1.0;
            //! initialize window for N1 and N2, then find coincidences
            const size_t true_coinc = xp.true_coincidences(N1, N2, a, b, delta);
            fp(" %u", unsigned(true_coinc) );
            
            __CHRONO(ell_perm,xp.bootstrap(Bcoinc, bootstrap_perm, N1, N2, delta));
            Stats::AverageAndStdErr(ave,sig,Bcoinc);
            fp(" %g %g", ave, sig);
            
            __CHRONO(ell_repl,xp.bootstrap(Bcoinc, bootstrap_repl, N1, N2, delta));
            Stats::AverageAndStdErr(ave,sig,Bcoinc);
            fp(" %g %g", ave, sig);
            std::cerr << "."; std::cerr.flush();
            if( 0 == (count % 16 ) ) std::cerr << std::endl;
            fp("\n");
        }
        std::cerr << std::endl;
        const double num_bs = (nb * count) * 1e-6;
        std::cerr << "speed of perm=" << num_bs/ell_perm << " M/s" << std::endl;
        std::cerr << "speed of repl=" << num_bs/ell_repl << " M/s" << std::endl;

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
