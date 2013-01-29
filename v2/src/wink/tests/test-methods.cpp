#include "../neurons.hpp"
#include "../../pyck/wtime.hpp"
#include "../../pyck/stats.hpp"

#include <iostream>

using namespace wink;

#define __CHRONO(ELL,CODE) do { const double tmx = chrono.query(); CODE; ELL += chrono.query() - tmx; } while(false)

static inline double delta_sq(double a,double b)
{
    const double d = a-b;
    return d*d;
}

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
        
        
        size_t           count = 0;
        const size_t     B     = 5000;
        C_Array<count_t> coinc0(B);
        C_Array<count_t> coinc1(B);
        double am0=0, ap0=0;
        double am1=0, ap1=0;
        
        
        double ell_T0 = 0;
        double ell_T1 = 0;
        
        double rms_Tm = 0;
        double rms_Tp = 0;
        
        double ell_H0 = 0;
        double ell_H1 = 0;
        
        double rms_Hm = 0;
        double rms_Hp = 0;
        
        std::cerr << "Evalutating" << std::endl;
        for( double a=0;a<10; a += 0.1 )
        {
            ++count;
            std::cerr << "."; std::cerr.flush(); if( 0 == (count%20) ) std::cerr << std::endl;
            const double b = a + 1;
            count_t T0=0,T1=0;
            
            __CHRONO(ell_T0,
                     T0=xp.true_coincidences( statistic_T, N1, N2, a, b, delta);
                     xp.mix(statistic_T, coinc0, mix_perm, N1, N2, delta) );
            xp.compute_pvalues(am0, ap0, coinc0, T0);
            
            __CHRONO(ell_T1,
                     T1=xp.self_coincidences(statistic_T, N1, N2, a, b, delta);
                     xp.eval_coincidences(statistic_T, coinc1, mix_perm) );
            
            xp.compute_pvalues(am1, ap1, coinc1, T1);
            
            if( T0 != T1 ) throw Exception("T0!=T1");
            
            //std::cerr << am0 << "/" << am1 << "  |  " << ap0 << "/" << ap1 << std::endl;
            rms_Tm += delta_sq(am0,am1);
            rms_Tp += delta_sq(ap0,ap1);
            
            count_t H0=0,H1=0;
            
            __CHRONO(ell_H0,
                     H0=xp.true_coincidences( statistic_H, N1, N2, a, b, delta);
                     xp.mix(statistic_H, coinc0, mix_perm, N1, N2, delta) );
            xp.compute_pvalues(am0, ap0, coinc0, H0);
            
            __CHRONO(ell_H1,
                     H1=xp.self_coincidences(statistic_H, N1, N2, a, b, delta);
                     xp.eval_coincidences(statistic_H, coinc1, mix_perm) );
            
            xp.compute_pvalues(am1, ap1, coinc1, H1);
            
            if( H0 != H1 ) throw Exception("H0!=H1");
            
            rms_Hm += delta_sq(am0,am1);
            rms_Hp += delta_sq(ap0,ap1);
        }
        std::cerr << std::endl;
        
        rms_Tm = sqrt(rms_Tm) / count;
        rms_Tp = sqrt(rms_Tp) / count;
        
        const double fac = double(count) * double(B) * 1e-6;
        
        std::cerr << "speed T0 (M/s) = " << fac/ell_T0 << std::endl;
        std::cerr << "speed T1 (M/s) = " << fac/ell_T1 << std::endl;
        std::cerr << "            rms= " << rms_Tm << "/" << rms_Tp << std::endl;
        std::cerr << "speed up       = " << ell_T0 / ell_T1 << std::endl;
        
        std::cerr << std::endl;
        rms_Hm = sqrt(rms_Hm) / count;
        rms_Hp = sqrt(rms_Hp) / count;
        std::cerr << "speed H0 (M/s) = " << fac/ell_H0 << std::endl;
        std::cerr << "speed H1 (M/s) = " << fac/ell_H1 << std::endl;
        std::cerr << "            rms= " << rms_Hm << "/" << rms_Hp << std::endl;
        std::cerr << "speed up       = " << ell_H0 / ell_H1 << std::endl;

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

