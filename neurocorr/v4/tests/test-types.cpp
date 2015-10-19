#include "../types.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/code/rand.hpp"

YOCTO_UNIT_TEST_IMPL(types)
{

    Converter cv(1e3);
    for(size_t i=0;i<10;++i)
    {
        const Real x = alea<Real>();
        const Unit u = cv.toUnit(x);
        const Real y = cv.toReal(u);
        std::cerr << x << "->" << u << "->" << y << std::endl;
    }


}
YOCTO_UNIT_TEST_DONE()
