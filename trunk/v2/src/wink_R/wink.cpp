#include "../pyck/R++.hpp"

extern "C" SEXP wink_version()
{
    Rprintf("0.1 Compiled on %s\n", __DATE__);
    return R_NilValue;
}