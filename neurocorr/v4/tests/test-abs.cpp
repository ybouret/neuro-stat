#include "../cpw.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/sys/wtime.hpp"

static inline
void do_test( const CPW &F )
{
    std::cerr << "F.size=" << F.size() << std::endl;
    
    uint64_t raw64 = 0;
    uint64_t opt64 = 0;
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


}

YOCTO_UNIT_TEST_IMPL(abs)
{
    CPW F;

    do_test(F);

    F.add(0,1);
    do_test(F);

}
YOCTO_UNIT_TEST_DONE()


