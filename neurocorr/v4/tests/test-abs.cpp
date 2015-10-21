#include "../cpw.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/sys/wtime.hpp"

static inline
void do_test( const CPW &F , uint64_t &raw64, uint64_t &opt64)
{
    std::cerr << std::endl << "F.size=" << F.size() << std::endl;
    
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
            //std::cerr << ".";
            const Unit tauEnd = tau+w;
            mark = chrono.ticks();
            const Unit raw    = F.maxAbsOn_(tau,tauEnd);
            raw64 += chrono.ticks() - mark;

            mark = chrono.ticks();
            const Unit opt    = F.maxAbsOn(tau,tauEnd);
            opt64 += chrono.ticks() - mark;

            if(raw!=opt)
            {
                std::cerr << "Bad!" << std::endl;
                std::cerr << "raw=" << raw << std::endl;
                std::cerr << "opt=" << opt << std::endl;
                std::cerr << "tauStart=" << tau << std::endl;
                std::cerr << "tauFinal=" << tauEnd << std::endl;
                std::cerr << "foot=" << F.foot << std::endl;
                std::cerr << "points=" << F << std::endl;
                throw exception("Mismatch :raw=%ld, opt=%ld!", long(raw), long(opt) );
            }
        }
    }
    const double rawTime = chrono(raw64);
    const double optTime = chrono(opt64);
    std::cerr << "rawTime=" << rawTime << std::endl;
    std::cerr << "optTime=" << optTime << std::endl;
}

#include "yocto/code/rand.hpp"

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

    for(size_t np=2;np<=50;++np)
    {
        raw64 = opt64 = 0;
        for(size_t iter=0;iter<1;++iter)
        {
            F.free();
            Unit curr = 0;
            for(size_t i=1;i<=np;++i)
            {
                curr += 1 + Unit(alea_leq(5));
                const Unit val = -10 + Unit(alea_leq(20));
                F.add(curr,val);
            }
            do_test(F,raw64,opt64);
        }
        const double speedUp = double(raw64)/double(opt64);
        std::cerr << "\tSpeedUp" << np << " : " << speedUp << std::endl;
    }


}
YOCTO_UNIT_TEST_DONE()


