#include "../cpwfn.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/code/rand.hpp"
#include "yocto/ios/ocstream.hpp"
#include "yocto/sys/timings.hpp"

#define TEST_TIME 2.0

YOCTO_UNIT_TEST_IMPL(cpwfn)
{

    const Real delta = 1000.0;
    timings tmx;
    for(size_t iter=0;iter<1000;++iter)
    {
        CPW_Function fn(delta);

        Unit tmp = 0;
        for(size_t i=1+alea_lt(100);i>0;--i)
        {
            const Unit u = 1+alea_lt(100);
            tmp += u;
            fn.insert(tmp,Unit(alea_leq(100))-50);
        }

        const Unit tauLo = fn.front().tau-5;
        const Unit tauHi = fn.back().tau +5;
        if(iter<=0)
        {
            fn.saveTo("cpw.dat");

            ios::wcstream fp("cpw_eval.dat");
            for(Unit tau=tauLo;tau<=tauHi;++tau)
            {
                fp("%ld %ld\n", long(tau), long(fn(tau)) );
            }
        }

        const size_t  np = tauHi-tauLo+1;
        CVector<Unit> points(np);
        UList         plist(np,as_capacity);
        for(Unit tau=tauLo,i=0;tau<=tauHi;++i,++tau)
        {
            points[i] = tau;
            plist.push_back(tau);
        }

        const Unit uRaw = fn._sumValuesAtOrdered(points);
        const Unit lRaw = fn._sumValuesAtOrdered(plist);
        if(uRaw!=lRaw)
        {
            throw exception("_sumValuesAtOrdered failure!");
        }
        const Unit uOpt = fn. sumValuesAtOrdered(points);
        const Unit lOpt = fn. sumValuesAtOrdered(plist);
        if(uOpt!=lOpt)
        {
            throw exception("_sumValuesAtOrdered failure!");
        }

        //std::cerr << uRaw << "/" << uOpt << std::endl;
        if(uRaw!=uOpt)
        {
            throw exception("sumValuesAtOrdered failure!");
        }

        if(iter<=0)
        {
            std::cerr << "Timings Each method for " << TEST_TIME << " seconds..." << std::endl;
            volatile Unit theSum = 0;
            YOCTO_TIMINGS(tmx, TEST_TIME, theSum += fn._sumValuesAtOrdered(points));
            const double rawSpeed = tmx.speed;
            std::cerr << "rawSpeedCore=" << rawSpeed << std::endl;
            
            YOCTO_TIMINGS(tmx, TEST_TIME, theSum += fn._sumValuesAtOrdered(plist));
            const double rawSpeedList = tmx.speed;
            std::cerr << "rawSpeedList=" << rawSpeedList << std::endl;
            
            YOCTO_TIMINGS(tmx, TEST_TIME, theSum += fn.sumValuesAtOrdered(points));
            const double optSpeed = tmx.speed;
            std::cerr << "optSpeedCore=" << optSpeed << std::endl;
            YOCTO_TIMINGS(tmx, TEST_TIME, theSum += fn.sumValuesAtOrdered(plist));
            const double optSpeedList = tmx.speed;
            std::cerr << "optSpeedList=" << optSpeedList << std::endl;

            std::cerr << "CoreSpeedUp=" << optSpeed/rawSpeed         << std::endl;
            std::cerr << "ListSpeedUp=" << optSpeedList/rawSpeedList << std::endl;
        }
    }


    for(size_t iter=0;iter<10000;++iter)
    {
        const size_t  nmax = 10;
        CMatrix<Real> nd(1,1+nmax);
        const size_t  ns = alea_leq(nmax);
        nd[0][0] = ns;
        size_t j = 0;
        for(size_t i=1;i<=ns;++i)
        {
            j += 1+alea_lt(4);
            nd[0][i] = j;
        }
        Train tr(1,nd,0);
        if(iter<=0)
        {
            ios::wcstream fp("train.dat");
            for(size_t i=0;i<tr.size();++i)
            {
                fp("%ld 0\n", long(tr[i]));
            }
        }

        CPW_Function L(delta);
        L.buildFrom(tr,1,false);
        if(iter<=0)
        {
            L.saveTo("Lraw.dat");
            L.removeEmptyIntervals();
            L.saveTo("L.dat");
        }


        CPW_Function R(delta);
        R.buildFrom(tr,2,false);
        if(iter<=0)
        {
            R.saveTo("Rraw.dat");
            R.removeEmptyIntervals();
            R.saveTo("R.dat");
            R.shiftBy(1);
            R.saveTo("R1.dat");
        }

        CPW_Function prod(L,R);
        if(iter<=0)
        {
            prod.saveTo("Prod.dat");
        }
    }
    
}
YOCTO_UNIT_TEST_DONE()
