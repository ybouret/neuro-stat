#include "../records.hpp"
#include "yocto/utest/run.hpp"

YOCTO_UNIT_TEST_IMPL(records)
{
    auto_ptr<Records> pRec( Records::GenerateRandom(5, 3, 100, 5) );
    Records &records = *pRec;
    records.display();
}
YOCTO_UNIT_TEST_DONE()
