#include "record.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/math/kernel/matrix.hpp"
#include "yocto/code/rand.hpp"

using namespace math;

YOCTO_UNIT_TEST_IMPL(records)
{

    for(size_t iter=0;iter<1000;++iter)
    {
        const size_t nr = 1+alea_lt(10);
        const size_t nc = 1+alea_leq(100);

        matrix<double> data(nr,nc);
        Records        records(nr);
        for(size_t i=1;i<=nr;++i)
        {
            data[i][1] = alea_lt(nc);
            records.Append(data[i]);
        }
    }

}
YOCTO_UNIT_TEST_DONE()
