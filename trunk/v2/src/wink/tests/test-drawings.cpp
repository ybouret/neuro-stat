#include "../../pyck/rand32.hpp"
#include "../../pyck/wtime.hpp"
#include "../../wink/drawing.hpp"
#include "../../pyck/c_array.hpp"
#include "../../pyck/stats.hpp"

#include <iostream>

using namespace wink;



static inline
void display_stats( const C_Array<double> &arr, const size_t n )
{
    double ave=0, sig=0;
    Stats::AverageAndStdDev(ave, sig, arr);
    ave /= n-1;
    sig /= n-1;
    std::cerr << "Average=" << ave << ", StdDev=" << sig << std::endl;
}

static inline
void perform( drawing &drw, DefaultUniformGenerator &ran)
{
    const size_t    n = 100;
    const size_t    N = 4000;
    C_Array<double> a_head(N);
    C_Array<double> a_tail(N);
    C_Array<double> a_mid(N);
    for( size_t i=0; i < N; ++i )
    {
        drw.permutation(n, ran);
        a_head[i] = drw.head->second; assert(drw.head->first==0);
        a_tail[i] = drw.tail->second; assert(drw.tail->first==n-1);
        a_mid[i]  = drw.fetch(n/2)->second;
    }
    std::cerr << "Head: " << std::endl;
    display_stats(a_head,n);
    std::cerr << "Tail: " << std::endl;
    display_stats(a_tail,n);
    std::cerr << "Middle: " << std::endl;
    display_stats(a_mid,n);
    
}

int main(int argc, char *argv[] )
{
    try
    {

        DefaultUniformGenerator ran;
        ran.seed( WallTime::Seed() );
        drawing drw;
        drw.reserve(1000);
        drw.identity(10);
        std::cerr << "drw=" << drw << std::endl;
        std::cerr << std::endl;
        for( size_t i=0; i < 10; ++i )
        {
            drw.permutation(10,ran);
            std::cerr << "drw=" << drw << std::endl;
        }
        std::cerr << std::endl;
        for( size_t i=0; i < 10; ++i )
        {
            drw.replacement(10,ran);
            std::cerr << "drw=" << drw << std::endl;
        }
        std::cerr << std::endl;

        
        perform( drw, ran );
        
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
