#include "yocto/R/R++.hpp"
#include "neuron.hpp"
#include "yocto/ptr/auto.hpp"

using namespace yocto;

////////////////////////////////////////////////////////////////////////////////
//
// C++ interface to R
//
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
//
//
// Used to check I/O
//
//______________________________________________________________________________
extern "C"
SEXP NeuroCorr_Version()
{
    Rprintf("Compiled on %s\n", __DATE__);
    return R_NilValue;
}
