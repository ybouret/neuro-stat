#include "../cpw.hpp"
#include "yocto/utest/run.hpp"

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
}

YOCTO_UNIT_TEST_IMPL(intg)
{

    CPW F;

    F.foot = 1;

    do_intg(F);

}
YOCTO_UNIT_TEST_DONE()

