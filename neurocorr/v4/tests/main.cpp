#include "yocto/utest/driver.hpp"

YOCTO_UNIT_TEST_INIT(16)
{
    YOCTO_UNIT_TEST_DECL(types);
    YOCTO_UNIT_TEST_DECL(records);
    YOCTO_UNIT_TEST_DECL(cpw);
    YOCTO_UNIT_TEST_DECL(phi);
    YOCTO_UNIT_TEST_DECL(locate);
    YOCTO_UNIT_TEST_DECL(moments);
    YOCTO_UNIT_TEST_DECL(abs);
}
YOCTO_UNIT_TEST_EXEC()
