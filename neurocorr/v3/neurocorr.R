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
## CheckData
##
################################################################################
NeuroCorr_CheckData <- function( dataNeur, numNeurones, scale )
{
    if( !is.matrix(dataNeur) )    stop("[CheckData] : dataNeur must be a matrix");
    if( !is.double(numNeurones) ) stop("[CheckData] : numNeurones must be a number");
    if( !is.double(scale)       ) stop("[CheckData] : scale must be a number");
    .Call("NeuroCorr_CheckData",dataNeur,numNeurones,scale);
}

################################################################################
##
## Compute a Phi Function
##
################################################################################
NeuroCorr_ComputePhi0 <- function(dataNeur,numNeurones,scale,delta)
{
    if( !is.matrix(dataNeur) )    stop("[ComputePhi0] : dataNeur must be a matrix");
    if( !is.double(numNeurones) ) stop("[ComputePhi0] : numNeurones must be a number");
    if( !is.double(scale)       ) stop("[ComputePhi0] : scale must be a number");
    if( !is.double(delta)       ) stop("[ComputePhi0] : delta must be a number");

    .Call("NeuroCorr_ComputePhi0",dataNeur,numNeurones,scale,delta);
}

