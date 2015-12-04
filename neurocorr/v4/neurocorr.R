################################################################################
##
## R interface to C++
##
################################################################################

NeuroCorr <- paste("neurocorr", .Platform$dynlib.ext, sep="")
print(paste("NeuroCorr: Loading NeuroCorr from ",NeuroCorr) )

## load the dll only once
if( !is.loaded("NeuroCorr_Version") )
{
	if( .Platform$OS.type != "windows" )
	{
		dyn.unload(NeuroCorr);
	}
	dyn.load(NeuroCorr);
	if( !is.loaded("NeuroCorr_Version") ) stop("NeuroCorr: Unable to find NeuroCorr_Version!");
}


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
## Set Internal #Threads to use
##
################################################################################
NeuroCorr_SetParallel <- function( numThreads )
{
    if( !is.double(numThreads) ) stop("[SetParallel] : scale must be a number");
    .Call("NeuroCorr_SetParallel", numThreads);
}

################################################################################
##
## Check we got some proper data
##
################################################################################
NeuroCorr_CheckData <- function(dataNeur,numNeurones,scale)
{
    if( !is.matrix(dataNeur) )    stop("[CheckData] : dataNeur must be a matrix");
    if( !is.double(numNeurones) ) stop("[CheckData] : numNeurones must be a number");
    if( !is.double(scale)       ) stop("[CheckData] : scale must be a number");
    .Call("NeuroCorr_CheckData",dataNeur,numNeurones,scale);
}


################################################################################
##
## Return the graph of Phi0, mostly for testing
##______________________________________________________________________________
##
## INPUT
##______________________________________________________________________________
##
## train : a vector of spike times
## scale : the desired scaling (scale>=1)
## delta : the delta value, in units of times
##
##______________________________________________________________________________
##
## OUTPUT
##______________________________________________________________________________
##
## a matrix with 2 columns: partition and values of the partition
##
################################################################################
NeuroCorr_Phi0 <- function(train,scale,delta)
{
    if( !is.vector(train) ) stop("[Phi0] : train must be a vector");
    if( !is.double(scale) ) stop("[Phi0] : scale must be a number");
    if( !is.double(delta) ) stop("[Phi0] : delta must be a number");
    .Call("NeuroCorr_Phi0",train,scale,delta);
}


################################################################################
##
## Compute Matrices
##______________________________________________________________________________
##
## INPUT
##______________________________________________________________________________
##
## dataNeur   : a matrix of #trains * (1+maxSpikes) reals
## numNeurones: number of neurones, so that #trials = #trains/neurones
## scale      : the scale of spike times: use 1e6 to get microseconds and so on
## delta      : counting interval size in spikes times value
## K          : number of pieces for the Phi functions
## boxes      : the boxes, each column is (#trial,start_time,final_time,kind)
## grouping   : "byKind" of "byBox"
##
## When using "byKind", the matrices are returned by INCREASING kind values.
##
##______________________________________________________________________________
##
## OUTPUT
##______________________________________________________________________________
##
## A Main List of Sub-Lists.
## Each Sub-List contains the number of matrices corresponding to
## the 'grouping' choice, and are labelled '1'...'#groups'.
##
## REMARK: When using 'byKind', the label is the kind IFF the kinds
## are continuous and starting from 1...
##
## - The main list contains 4 Sub-Lists:
## |_$mu1: (1+neurones*K)*neurones, counting values,   first line is #count
## |_$mu2: (1+neurones*K)*neurones, counting values^2, first line is #count, same as mu1
## |_$muA: (1+neurones*K)*1, max |count|, first  line is 1
## |_$G:   (1+neurones*K)*(1+neurones*L), values of integrals
##
################################################################################
NeuroCorr_Compute <- function(dataNeur,numNeurones,scale,delta,K,boxes,grouping)
{
    if( !is.matrix(dataNeur)    ) stop("[Compute] : dataNeur must be a matrix");
    if( !is.double(numNeurones) ) stop("[Compute] : numNeurones must be a number");
    if( !is.double(scale)       ) stop("[Compute] : scale must be a number");
    if( !is.double(delta)       ) stop("[Compute] : delta must be a number");
    if( !is.double(K)           ) stop("[Compute] : K must be a number");
    if( !is.character(grouping) ) stop("[Compute] : grouping must be 'byKind' or 'byBox'");
    if( !is.matrix(boxes)       ) stop("[Compute] : boxes must be a matrix (4 lines, #boxes columns)");

    .Call("NeuroCorr_Compute",dataNeur,numNeurones,scale,delta,K,boxes,grouping);

}

