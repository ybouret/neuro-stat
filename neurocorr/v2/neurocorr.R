################################################################################
##
## R interface to C++
##
################################################################################

neurocorr_dll <- paste("neurocorr", .Platform$dynlib.ext, sep="")
print(paste("NeuroCorr: Loading NeuroCorr from ",neurocorr_dll) )

## load the dll only once
## Warning, Buggy for WINDOWS !!!
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

################################################################################
##
## Set Internal Threads
##
################################################################################
NeuroCorr_SetParallel <- function( numProcs )
{
    if( !is.double(numProcs) ) stop("SetParallel: numProcs is not a number");
    .Call("NeuroCorr_SetParallel",numProcs);
}

################################################################################
##
## CleanUp internal data
##
################################################################################
NeuroCorr_CleanUp <- function()
{
    .Call("NeuroCorr_CleanUp");
}


################################################################################
##
## internally load the NeuroData
##
################################################################################

NeuroCorr_CheckNeuroData <- function( neuroData, numNeurones, scale )
{
    if( !is.matrix(neuroData)    ) stop("CheckNeuroData: neuroData   is not a matrix");
    if( !is.double(numNeurones)  ) stop("CheckNeuroData: numNeurones is not a number");
    if( !is.double(scale)        ) stop("CheckNeuroData: scale       is not a number");

    .Call("NeuroCorr_CheckNeuroData", neuroData, numNeurones, scale);
}
