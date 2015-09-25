#include "yocto/utest/driver.hpp"

YOCTO_UNIT_TEST_INIT(8)
{
    YOCTO_UNIT_TEST_DECL(types);
    YOCTO_UNIT_TEST_DECL(train);
    YOCTO_UNIT_TEST_DECL(cpwfn);
    YOCTO_UNIT_TEST_DECL(intg);
    YOCTO_UNIT_TEST_DECL(records);
    YOCTO_UNIT_TEST_DECL(phifn);
    YOCTO_UNIT_TEST_DECL(box);

}
YOCTO_UNIT_TEST_EXEC()
