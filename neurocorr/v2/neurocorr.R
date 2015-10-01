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


NeuroCorr_ComputePhi <- function( neuroData, numNeurones, scale, delta, K)
{
    if( !is.matrix(neuroData)    ) stop("ComputePhi: neuroData   is not a matrix");
    if( !is.double(numNeurones)  ) stop("ComputePhi: numNeurones is not a number");
    if( !is.double(scale)        ) stop("ComputePhi: scale       is not a number");
    if( !is.double(delta)        ) stop("ComputePhi: delta       is not a number");
    if( !is.double(K)            ) stop("ComputePhi: K           is not a number");
    
    .Call("NeuroCorr_ComputePhi",neuroData,numNeurones,scale,delta,K);
}

