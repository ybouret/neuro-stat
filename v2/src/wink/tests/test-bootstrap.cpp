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
        
        C_Array<count_t> Bcoinc(4096);
        const size_t     nb    = Bcoinc.size;
        const double     delta = 0.2;
        cFile fp_perm_T("coinc_perm_T.dat", cFile::Overwrite);
        cFile fp_repl_T("coinc_repl_T.dat", cFile::Overwrite);
        cFile fp_perm_H("coinc_perm_H.dat", cFile::Overwrite);
        cFile fp_repl_H("coinc_repl_H.dat", cFile::Overwrite);

        chrono.start();
        double ell_perm_T  = 0;
        double ell_repl_T  = 0;
        double ell_perm_H  = 0;
        double ell_repl_H  = 0;
        size_t count       = 0;
        double alpha_plus  = 0;
        double alpha_minus = 0;
        for( double a=0;a<10; a += 0.1 )
        {
            ++count;
            fp_perm_T("%g",a);
            fp_repl_T("%g",a);
            fp_perm_H("%g",a);
            fp_repl_H("%g",a);

            const double b          = a + 1.0;
            //! initialize window for N1 and N2, then find coincidences
            const count_t true_coinc_T = xp.true_coincidences(statistic_T, N1, N2, a, b, delta);
            fp_perm_T(" %d", int(true_coinc_T) );
            fp_repl_T(" %d", int(true_coinc_T) );
            
            const count_t true_coinc_H = xp.true_coincidences(statistic_H, N1, N2, a, b, delta);
            fp_perm_H(" %d", int(true_coinc_H) );
            fp_repl_H(" %d", int(true_coinc_H) );

            
            __CHRONO(ell_perm_T,xp.mix( statistic_T, Bcoinc, mix_perm, N1, N2, delta));
            xp.compute_pvalues(alpha_minus, alpha_plus, Bcoinc, true_coinc_T);
            fp_perm_T(" %g %g", alpha_minus, alpha_plus);
            
            __CHRONO(ell_repl_T,xp.mix( statistic_T, Bcoinc, mix_repl, N1, N2, delta));
            xp.compute_pvalues(alpha_minus, alpha_plus, Bcoinc, true_coinc_T);
            fp_repl_T(" %g %g", alpha_minus, alpha_plus);
            
            __CHRONO(ell_perm_H,xp.mix( statistic_H, Bcoinc, mix_perm, N1, N2, delta));
            xp.compute_pvalues(alpha_minus, alpha_plus, Bcoinc, true_coinc_H);
            fp_perm_H(" %g %g", alpha_minus, alpha_plus);
            
            __CHRONO(ell_repl_H,xp.mix( statistic_H, Bcoinc, mix_perm, N1, N2, delta));
            xp.compute_pvalues(alpha_minus, alpha_plus, Bcoinc, true_coinc_H);
            fp_repl_H(" %g %g", alpha_minus, alpha_plus);
            
            std::cerr << "."; std::cerr.flush();
            if( 0 == (count % 16 ) ) std::cerr << std::endl;
            fp_perm_T("\n");
            fp_repl_T("\n");
            fp_perm_H("\n");
            fp_repl_H("\n");
            
        }
        std::cerr << std::endl;
        const double num_bs = (nb * count) * 1e-6;
        std::cerr << "speed of perm_T=" << num_bs/ell_perm_T << " M/s" << std::endl;
        std::cerr << "speed of repl_T=" << num_bs/ell_repl_T << " M/s" << std::endl;
        std::cerr << "speed of perm_H=" << num_bs/ell_perm_H << " M/s" << std::endl;
        std::cerr << "speed of repl_H=" << num_bs/ell_repl_H << " M/s" << std::endl;

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
