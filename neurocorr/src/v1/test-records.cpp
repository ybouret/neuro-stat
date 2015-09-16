#include "record.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/code/rand.hpp"

//using namespace math;

YOCTO_UNIT_TEST_IMPL(records)
{

    for(size_t iter=0;iter<10000;++iter)
    {
        const size_t nr = 1+alea_lt(100);
        const size_t nc = 1+alea_leq(100);

        CMatrix<double> data(nr,nc);
        Records         records(nr);
        for(size_t i=0;i<nr;++i)
        {
            data[i][0] = alea_lt(nc);
            records.Append(data[i]);
        }
    }

}
YOCTO_UNIT_TEST_DONE()
