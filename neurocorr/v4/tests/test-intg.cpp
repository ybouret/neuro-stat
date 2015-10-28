#include "../cpw.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/ios/ocstream.hpp"

static inline
void do_intg( const CPW &F )
{
    std::cerr << "F.size=" << F.size() << std::endl;
    Unit tauMin = 0;
    Unit tauMax = 0;
    if(F.size()<=0)
    {
        tauMin = -5;
        tauMax =  5;
    }
    else
    {
        tauMin = F.front().tau-5;
        tauMax = F.back() .tau+5;
    }
    
    const string fn = vformat("f%u.dat", unsigned(F.size()));
    F.save(fn.c_str());
    ios::wcstream fp( vformat("intg%u.dat", unsigned(F.size()) ) );
    for(Unit tau=tauMin;tau<=tauMax;++tau)
    {
        fp("%ld",long(tau));
        for(Unit w=1;w<=5;++w)
        {
            const Unit ans = F.integrate_(tau, tau+w);
            const Unit res = F.integrate(tau,tau+w);
            fp(" %ld", long(ans) );
            if(ans!=res)
            {
                std::cerr << "!";
                //throw exception("integrate mismatch, F.size=%u", unsigned(F.size()));
            }
        }
        fp("\n");
    }
}

#include "yocto/sys/wtime.hpp"

static inline
void perf_intg(const CPW &F, uint64_t &raw64, uint64_t &opt64, wtime &chrono )
{
    uint64_t mark = 0;


    Unit tauMin = 0;
    Unit tauMax = 0;
    if(F.size()<=0)
    {
        tauMin = -10;
        tauMax =  10;
    }
    else
    {
        tauMin = F.front().tau-10;
        tauMax = F.back() .tau+10;
    }
    const Unit W = (tauMax-tauMin);
    for(Unit tauStart=tauMin;tauStart<=tauMax;++tauStart)
    {
        for(Unit w=1;w<=W;++w)
        {
            const Unit tauFinal = tauStart+w;
            if(tauFinal>tauMax)
                break;

            mark = chrono.ticks();
            const Unit raw = F.integrate_(tauStart,tauFinal);
            raw64 += chrono.ticks()-mark;

            mark = chrono.ticks();
            const Unit opt = F.integrate(tauStart,tauFinal);
            opt64 += chrono.ticks()-mark;

            if(raw!=opt)
            {
                throw exception("intg mismatch");
            }
        }
    }

}

static inline double display( const uint64_t r, const uint64_t o, wtime &chrono)
{
    const double rTime = chrono(r);
    const double oTime = chrono(o);
    std::cerr << "raw_time=" << rTime << std::endl;
    std::cerr << "opt_time=" << oTime << std::endl;
    const double ans = rTime/oTime;
    std::cerr << "speed_up=" << ans << std::endl;
    return ans;
}

#include "yocto/code/rand.hpp"

YOCTO_UNIT_TEST_IMPL(intg)
{
    wtime   chrono;
    chrono.start();

    CPW F;

    F.foot = 1;
    do_intg(F);
    
    F.add(0,2);
    do_intg(F);

    F.add(2,3);
    do_intg(F);

    F.add(6,4);
    do_intg(F);

    F.add(8,5);
    do_intg(F);

    std::cerr << "perf/check..." << std::endl;
    uint64_t raw64 = 0;
    uint64_t opt64 = 0;

    ios::ocstream::overwrite("intgperf.dat");
    F.free();
    F.foot = 1;
    for(size_t n=0;n<=30;++n)
    {
        const size_t num_iter = 10 + 1000/((n+1)*(n+1));
        std::cerr << "#iter=" << num_iter << std::endl;
        for(size_t iter=0;iter<num_iter;++iter)
        {
            F.free();
            Unit curr = 0;
            for(size_t i=0;i<n;++i)
            {
                curr += 1+alea_leq(10);
                F.add(curr,i+2);
            }
            perf_intg(F,raw64,opt64,chrono);
        }
        std::cerr << "-- n=" << n << std::endl;
        const double ans = display(raw64,opt64,chrono);
        ios::acstream fp("intgperf.dat");
        fp("%u %g\n", unsigned(n), ans);
        raw64 = 0;
        opt64 = 0;
    }


}
YOCTO_UNIT_TEST_DONE()

