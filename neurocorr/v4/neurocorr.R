################################################################################
##
## R interface to C++
##
################################################################################

NeuroCorr <- paste("neurocorr", .Platform$dynlib.ext, sep="")
print(paste("NeuroCorr: Loading NeuroCorr from ",NeuroCorr) )

## load the dll only once
## Warning, Buggy for WINDOWS !!!
if( is.loaded("NeuroCorr_Version") ) dyn.unload(NeuroCorr);
dyn.load(NeuroCorr);
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
