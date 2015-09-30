#include "../types.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/code/rand.hpp"

YOCTO_UNIT_TEST_IMPL(types)
{
    
    Object obj(1000);
    for(size_t iter=0;iter<10;++iter)
    {
        const Real r = alea<double>();
        const Unit u = obj.toUnit(r);
        const Real s = obj.toReal(u);
        std::cerr << r << "->" << u << "->" << s << std::endl;
    }
}
YOCTO_UNIT_TEST_DONE()

