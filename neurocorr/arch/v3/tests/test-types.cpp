#include "../types.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/code/rand.hpp"

YOCTO_UNIT_TEST_IMPL(types)
{
    const Converter obj(1000.0);
    for(size_t i=0;i<10;++i)
    {
        const double x = alea<double>();
        const double u = obj.toUnit(x);
        const double y = obj.toReal(u);
        std::cerr << x << " -> " << u << "->" << y << std::endl;
    }
}
YOCTO_UNIT_TEST_DONE()
