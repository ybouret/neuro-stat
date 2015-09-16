#include "neurodata.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/code/rand.hpp"

//using namespace math;

YOCTO_UNIT_TEST_IMPL(neurodata)
{

    {
        const size_t nr = 1 + alea_leq(100);
        const size_t nt = 1 + alea_leq(100);
        const size_t ni = 1 + alea_leq(1000);

        NeuroData ND(nr,nt,ni);

    }


}
YOCTO_UNIT_TEST_DONE()