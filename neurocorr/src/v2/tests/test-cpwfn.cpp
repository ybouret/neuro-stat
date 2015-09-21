#include "../cpwfn.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/code/rand.hpp"
#include "yocto/ios/ocstream.hpp"

YOCTO_UNIT_TEST_IMPL(cpwfn)
{
    CPW_Function fn(1000.0,100);

    Unit tmp = 0;
    for(size_t i=1+alea_lt(100);i>0;--i)
    {
        const Unit u = 1+alea_lt(100);
        tmp += u;
        fn.insert(tmp,Unit(alea_leq(100))-50);
    }

    if(fn.size()>0)
    {
        ios::wcstream fp("cpw.dat");
        fp("%ld %ld\n", fn[1].tau, fn.foot);
        for(size_t i=1;i<fn.size();++i)
        {
            fp("%ld %ld\n", fn[i].tau,   fn[i].value);
            fp("%ld %ld\n", fn[i+1].tau, fn[i].value);
        }
        fp("%ld %ld\n", fn.back().tau, fn.back().value);
    }


    {
        const size_t  nmax = 100;
        CMatrix<Real> nd(1,1+nmax);

    }

}
YOCTO_UNIT_TEST_DONE()
