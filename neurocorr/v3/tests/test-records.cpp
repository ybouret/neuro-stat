#include "../records.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/code/rand.hpp"

YOCTO_UNIT_TEST_IMPL(records)
{
    auto_ptr<Records> pRec( Records::CreateRandom(10, 5, 1000, 3) );
    Records &records = *pRec;
    records.display();
    
}
YOCTO_UNIT_TEST_DONE()
