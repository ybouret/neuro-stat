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

    fn.saveTo("cpw.dat");
    


    //for(size_t iter=0;iter<10000;++iter)
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
        {
            ios::wcstream fp("train.dat");
            for(size_t i=0;i<tr.size();++i)
            {
                fp("%ld 0\n", tr[i]);
            }
        }

        fn.buildFrom(tr,1);
        fn.saveTo("phi1.dat");
        fn.buildFrom(tr,2);
        fn.saveTo("phi2.dat");
    }

}
YOCTO_UNIT_TEST_DONE()
