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


    const size_t maxN = 100;
    CPW F(maxN);

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
                fp(" %ld", long(F.integrate(tau,tau+w)) );
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
                fp(" %ld", long(F.integrate(tau,tau+w)) );
                
            }
            fp("\n");
        }

    }



}
YOCTO_UNIT_TEST_DONE()
