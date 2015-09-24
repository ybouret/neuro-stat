#include "yocto/utest/driver.hpp"

YOCTO_UNIT_TEST_INIT(16)
{
    YOCTO_UNIT_TEST_DECL(types);
    YOCTO_UNIT_TEST_DECL(train);
    YOCTO_UNIT_TEST_DECL(neuron);
    YOCTO_UNIT_TEST_DECL(cpwfn);
    YOCTO_UNIT_TEST_DECL(intg);
    YOCTO_UNIT_TEST_DECL(phi);
    YOCTO_UNIT_TEST_DECL(records);
    YOCTO_UNIT_TEST_DECL(phifn);
    
}
YOCTO_UNIT_TEST_EXEC()
