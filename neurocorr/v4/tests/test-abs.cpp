#include "../cpw.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/sys/wtime.hpp"

static inline
void do_test( const CPW &F , uint64_t &raw64, uint64_t &opt64)
{
    std::cerr << "F.size=" << F.size() << std::endl;
    
    uint64_t mark  = 0;

    wtime chrono;
    chrono.start();

    const Unit tauMin = ( F.size() > 0 ) ? F.front().tau - 10 : -10;
    const Unit tauMax = ( F.size() > 0 ) ? F.back().tau  + 10 : +10;
    const Unit wmax   = tauMax-tauMin;

    for(Unit tau=tauMin;tau<=tauMax;++tau)
    {
        for(Unit w=1;w<=wmax;++w)
        {
            std::cerr << ".";
            const Unit tauEnd = tau+w;
            mark = chrono.ticks();
            const Unit raw    = F.maxAbsOn_(tau,tauEnd);
            raw64 += chrono.ticks() - mark;

            mark = chrono.ticks();
            const Unit opt    = F.maxAbsOn(tau,tauEnd);
            opt64 += chrono.ticks() - mark;

            if(raw!=opt)
            {
                throw exception("Mismatch :raw=%ld, opt=%ld!", long(raw), long(opt) );
            }
        }
    }
    std::cerr << std::endl;
    const double rawTime = chrono(raw64);
    const double optTime = chrono(opt64);
    std::cerr << "rawTime=" << rawTime << std::endl;
    std::cerr << "optTime=" << optTime << std::endl;
}


YOCTO_UNIT_TEST_IMPL(abs)
{
    uint64_t opt64 = 0;
    uint64_t raw64 = 0;
    CPW F;

    F.foot = 1;
    do_test(F,raw64,opt64);

    raw64 = opt64 = 0;
    F.add(0,2);
    do_test(F,raw64,opt64);

    raw64 = opt64 = 0;
    F.add(1,3);
    do_test(F,raw64,opt64);
    

}
YOCTO_UNIT_TEST_DONE()


