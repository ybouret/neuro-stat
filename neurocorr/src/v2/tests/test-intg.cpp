#include "../cpwfn.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/ios/ocstream.hpp"

static inline
void do_intg(const char         *filename,
             const CPW_Function &F,
             const Unit          tauIni,
             const Unit          tauEnd)
{

    ios::wcstream fp(filename);
    const Unit w = (tauEnd-tauIni)/2;
    for(Unit tau=tauIni;tau<=tauEnd;++tau)
    {
        fp("%ld",tau);
        for(Unit h=1;h<=w;++h)
        {
            const Real res = F.integrate(tau-h, tau+h);
            fp(" %g", res);
        }
        fp("\n");
    }

}

YOCTO_UNIT_TEST_IMPL(intg)
{
    Real scale = 1000.0;

    //! value = 1
    CPW_Function fn(scale);
    fn.foot = 1;

    fn.saveTo("f0.dat");
    do_intg("intg0.dat",fn,-5,5);

    //! value = 0 before 0, 1 after
    fn.foot = 0;
    fn.insert(0, 1);
    fn.saveTo("f1.dat");
    do_intg("intg1.dat",fn,-5,5);

}
YOCTO_UNIT_TEST_DONE()

