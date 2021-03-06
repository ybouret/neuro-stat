#include "yocto/utest/driver.hpp"

YOCTO_UNIT_TEST_INIT(16)
{
    YOCTO_UNIT_TEST_DECL(types);
    YOCTO_UNIT_TEST_DECL(records);
    YOCTO_UNIT_TEST_DECL(cpw);
    YOCTO_UNIT_TEST_DECL(intg);
    YOCTO_UNIT_TEST_DECL(phi);
    YOCTO_UNIT_TEST_DECL(box);
}
YOCTO_UNIT_TEST_EXEC()
