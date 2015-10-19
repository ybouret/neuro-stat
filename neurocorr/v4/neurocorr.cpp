#include "yocto/R++/com.hpp"

YOCTO_R_FUNCTION(NeuroCorr_Version,())
{
    Rprintf("%s: compiled on %s\n", __fn, __DATE__ );
    return R_NilValue;
}
YOCTO_R_RETURN()
