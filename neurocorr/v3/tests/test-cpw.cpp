#include "../cpw.hpp"
#include "../records.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/code/rand.hpp"

YOCTO_UNIT_TEST_IMPL(cpw)
{
    CPW F(3);

    {
        F.save("f0.dat");

        F.insert(1,1);
        F.save("f1.dat");

        F.foot = 1;
        F.free();
        F.insert(1,-1);
        F.insert(2,F.foot);
        F.save("f2.dat");
    }

    auto_ptr<Records> pRec( Records::CreateRandom(1, 1, 10, 3) );
    const Train &train = *(*pRec)[0][0];
    std::cerr << "#spikes=" << train.size() << std::endl;
    train.save("train.dat");
    F.buildFrom(train,5);
    F.save("phi.dat");

}
YOCTO_UNIT_TEST_DONE()
