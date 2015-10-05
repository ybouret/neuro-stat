#include "../cpw.hpp"
#include "../records.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/code/rand.hpp"

#include "yocto/ios/ocstream.hpp"
#include "yocto/sys/wtime.hpp"
#include "yocto/string/conv.hpp"

YOCTO_UNIT_TEST_IMPL(intg)
{
    wtime chrono;
    chrono.start();
    size_t maxSpikes = 100;
    if(argc>1) maxSpikes = strconv::to<size_t>(argv[1],"maxSpikes");


    CPW F(5);

    //level 0
    const Unit wmax = 8;
    {
        F.free();
        F.foot = 1;
        F.save("f0.dat");
        ios::wcstream fp("i0.dat");
        for(Unit tau=-wmax;tau<=wmax;++tau)
        {
            fp("%ld", long(tau));
            for(Unit w=1;w<=wmax;++w)
            {
                const Unit ans = F.integrate(tau, tau+w);
                const Unit raw = F.integrate_(tau, tau+w);
                if(raw!=ans)
                {
                    throw exception("error Level 0, tau=%d, w=%d", int(tau), int(w));
                }
                fp(" %ld", long(ans) );
            }
            fp("\n");
        }
    }

    {
        F.insert(0,2);
        F.save("f1.dat");
        ios::wcstream fp("i1.dat");
        for(Unit tau=-wmax;tau<=wmax;++tau)
        {
            fp("%ld", long(tau));
            for(Unit w=1;w<=wmax;++w)
            {
                const Unit ans = F.integrate(tau, tau+w);
                const Unit raw = F.integrate_(tau, tau+w);
                if(raw!=ans)
                {
                    throw exception("error Level 1, tau=%d, w=%d", int(tau), int(w));
                }
                fp(" %ld", long(ans) );
            }
            fp("\n");
        }
    }

    {
        F.free();
        F.insert(0,-1);
        F.insert(1,2);
        F.save("f2.dat");
        ios::wcstream fp("i2.dat");
        for(Unit tau=-wmax;tau<=wmax;++tau)
        {
            fp("%ld", long(tau));
            for(Unit w=1;w<=wmax;++w)
            {
                const Unit ans = F.integrate(tau, tau+w);
                const Unit raw = F.integrate_(tau, tau+w);
                if(raw!=ans)
                {
                    throw exception("error Level 2, tau=%d, w=%d", int(tau), int(w));
                }
                fp(" %ld", long(ans) );
            }
            fp("\n");
        }

    }


    {
        F.free();
        F.insert(0,-1);
        F.insert(3,2);
        F.save("f2b.dat");
        ios::wcstream fp("i2b.dat");
        for(Unit tau=-wmax;tau<=wmax;++tau)
        {
            fp("%ld", long(tau));
            for(Unit w=1;w<=wmax;++w)
            {
                const Unit ans = F.integrate(tau, tau+w);
                const Unit raw = F.integrate_(tau, tau+w);
                if(raw!=ans)
                {
                    throw exception("error Level 2b, tau=%d, w=%d", int(tau), int(w));
                }
                fp(" %ld", long(ans) );
            }
            fp("\n");
        }

    }

    uint64_t rawClk = 0;
    uint64_t optClk = 0;
    uint64_t mark   = 0;
    std::cerr << "Benchmarking/testing..." << std::endl;
    for(size_t iter=0;iter<100;++iter)
    {
        auto_ptr<Records> pR( Records::CreateRandom(1,1,maxSpikes,3) );
        Records &records = *pR;

        const Unit W    = 10;
        const Unit tMin = records.tauMin-W;
        const Unit tMax = records.tauMax+W;
        const Unit dT   = 1+tMax-tMin;
        F.buildFrom(records(0),10);
        if(iter<=0)
        {
            F.save("fn.dat");
        }

        for(size_t sub=0;sub<100;++sub)
        {
            Unit a = tMin + alea_leq(dT);
            Unit b = tMin + alea_leq(dT);
            if(a>b) cswap(a,b);
            if(a<b)
            {
                mark = chrono.ticks();
                const Unit ans = F.integrate(a,b);
                optClk += chrono.ticks() - mark;

                mark = chrono.ticks();
                const Unit raw = F.integrate_(a,b);
                rawClk += chrono.ticks() - mark;
                //std::cerr << ans << "/" << raw << std::endl;
                if(ans!=raw)
                {
                    throw exception("integrate mismatch");
                }
            }
        }
        
        
    }

    const double rawTime = chrono(rawClk);
    const double optTime = chrono(optClk);

    std::cerr << "RawTime=" << rawTime << std::endl;
    std::cerr << "OptTime=" << optTime << std::endl;
    
    
    
    
}
YOCTO_UNIT_TEST_DONE()
