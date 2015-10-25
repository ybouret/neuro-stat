#include "../cpw.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/ios/ocstream.hpp"

static inline
void do_intg( const CPW &F )
{
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
            fp(" %ld", long(F.integrate_(tau, tau+w)) );
        }
        fp("\n");
    }
    
    
}

YOCTO_UNIT_TEST_IMPL(intg)
{

    CPW F;

    F.foot = 1;
    do_intg(F);
    
    F.add(0,2);
    do_intg(F);

}
YOCTO_UNIT_TEST_DONE()

