################################################################################
##
## R interface to C++
##
################################################################################

neurocorr_dll <- paste("neurocorr", .Platform$dynlib.ext, sep="")
print(paste("NeuroCorr: Loading",neurocorr_dll) )

## load the dll only once
if( is.loaded("NeuroCorr_Version") ) dyn.unload(neurocorr_dll);
dyn.load(neurocorr_dll)
if( !is.loaded("NeuroCorr_Version") ) stop("NeuroCorr: Unable to find NeuroCorr_Version!");


################################################################################
##
## display the version
##
################################################################################
NeuroCorr_Version <- function()
{
    .Call("NeuroCorr_Version");
}
