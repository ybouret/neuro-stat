#include "../cpw.hpp"
#include "yocto/utest/run.hpp"

YOCTO_UNIT_TEST_IMPL(cpw)
{
    CPW F;
    std::cerr << "f0=" << F << std::endl;
    F.save("f0.dat");

    F.add(0,1);
    std::cerr << "f1=" << F << std::endl;
    F.save("f1.dat");

    F.add(-1,2);
    std::cerr << "f2=" << F << std::endl;
    F.save("f2.dat");


}
YOCTO_UNIT_TEST_DONE()