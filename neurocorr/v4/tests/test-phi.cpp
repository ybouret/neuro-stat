#include "../phi.hpp"
#include "yocto/utest/run.hpp"

YOCTO_UNIT_TEST_IMPL(phi)
{
    auto_ptr<Records> pRec( Records::GenerateRandom(5, 3, 100, 5) );
    Records &records = *pRec;

    PHI_Functions Phi( records[1][1], 4 );
    Phi.build(8);

}
YOCTO_UNIT_TEST_DONE()
