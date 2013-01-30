#include "../neurons.hpp"
#include "../../pyck/wtime.hpp"

using namespace wink;
#define __CHRONO(ELL,CODE) do { const double tmx = chrono.query(); CODE; ELL += chrono.query() - tmx; } while(false)

int main(int argc, char *argv[] )
{
    try
    {
        WallTime     chrono;
        const size_t trials = 16;
        const size_t maxtop = 100;
        neurons      xp;
        
        chrono.start();
        
        neuron N1( trials, maxtop + xp.ran.leq(maxtop) );
        neuron N2( trials, maxtop + xp.ran.leq(maxtop) );
        
        for(size_t i=0; i < trials; ++i )
        {
            xp.ran.fill_array(0, 10, &N1[i][1], N1[i].size());
            xp.ran.fill_array(0, 10, &N2[i][1], N2[i].size());
        }
        
        C_Array<count_t> coinc(4096);
        const size_t     nb = coinc.size;
        
        size_t        count = 0;
        const  double delta = 0.2;
        
        double T_perm_ell = 0;
        double T_repl_ell = 0;
        
        cFile  T_perm_fp("T_perm.dat",cFile::Overwrite);
        cFile  T_repl_fp("T_repl.dat",cFile::Overwrite);
        
        double H_perm_ell = 0;
        
        cFile  H_perm_fp("H_perm.dat",cFile::Overwrite);
        
        
        double am=0,ap=0;
        for( double a=0;a<10; a += 0.1 )
        {
            ++count;
            std::cerr << "."; std::cerr.flush(); if( 0 == (count%10) ) std::cerr << std::endl;
            const double b = a+1.0;
            {
                count_t S = 0;
                __CHRONO(T_perm_ell,
                         S = xp.true_coincidences( statistic_T, N1, N2, a, b, delta);
                         xp.eval_coincidences(statistic_T, coinc, mix_perm)
                         );
                xp.compute_pvalues(am, ap, coinc, S);
                T_perm_fp("%g %g %g\n", a, am, ap);
            }
            
            {
                count_t S = 0;
                __CHRONO(T_repl_ell,
                         S = xp.true_coincidences( statistic_T, N1, N2, a, b, delta);
                         xp.eval_coincidences(statistic_T, coinc, mix_repl)
                         );
                xp.compute_pvalues(am, ap, coinc, S);
                T_repl_fp("%g %g %g\n", a, am, ap);
            }
            
            {
                count_t S = 0;
                __CHRONO(H_perm_ell,
                         S = xp.true_coincidences( statistic_H, N1, N2, a, b, delta);
                         xp.eval_coincidences(statistic_H, coinc, mix_perm)
                         );
                xp.compute_pvalues(am, ap, coinc, S);
                H_perm_fp("%g %g %g\n", a, am, ap);
            }
            
            
        }
        std::cerr << std::endl;
        
        const double fac = double(nb * count) * 1e-6;
        
        std::cerr << "speed T perm (M/s)=" << fac/T_perm_ell << std::endl;
        std::cerr << "speed T repl (M/s)=" << fac/T_repl_ell << std::endl;
        std::cerr << "speed H perm (M/s)=" << fac/H_perm_ell << std::endl;

        
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
