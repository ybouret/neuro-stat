#include "../cpwfn.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/ios/ocstream.hpp"
#include "yocto/code/rand.hpp"

static inline
void do_intg(const char         *filename,
             const CPW_Function &F,
             const Unit          tauIni,
             const Unit          tauEnd)
{

    ios::wcstream fp(filename);
    //std::cerr << "tauIni=" << tauIni << ", tauEnd=" << tauEnd << std::endl;
    const Unit w = 4;
    for(Unit tau=tauIni;tau<=tauEnd;++tau)
    {
        fp("%ld",long(tau));
        for(Unit h=1;h<=w;++h)
        {
            const Unit tauLo = tau-h;
            const Unit tauHi = tau+h;
            //std::cerr << "\t tauLo=" << tauLo << "-> tauHi=" << tauHi << std::endl;
            const Real res = F.integrate(tauLo,tauHi);
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
    do_intg("intg0.dat",fn,-10,10);

    //! value = 0 before 0, 1 after
    fn.foot = 0;
    fn.insert(0, 1);
    fn.saveTo("f1.dat");
    do_intg("intg1.dat",fn,-10,10);

    //! step finishing at tau=1
    fn.foot=1;
    fn.value(1)=-1;
    fn.insert(1,1);
    fn.saveTo("f2.dat");
    do_intg("intg2.dat",fn,-10,10);

    for(size_t iter=0;iter<20;++iter)
    {
        const size_t np    = 1 + alea_lt(100);
        const Unit   amp   = 50;
        const Unit   width = 4000;
        fn.clear();
        fn.foot = Unit(alea_leq(amp))-amp/2;
        for(size_t i=0;i<np;++i)
        {
            fn.insert(alea_leq(width),Unit(alea_leq(amp))-amp/2);
        }
        fn.removeEmptyIntervals();
        if(iter<=0)
        {
            fn.saveTo("fn.dat");
        }
        const Unit tauMin = -(width+10);
        const Unit tauMax = width+1;
        const Unit wMax   = 200;
        for(Unit w=0;w<=wMax;++w)
        {
            for(Unit tauLo=tauMin;tauLo<=tauMax;++tauLo)
            {
                (void) fn.integrate(tauLo,tauLo+w);
            }
        }
    }

}
YOCTO_UNIT_TEST_DONE()

