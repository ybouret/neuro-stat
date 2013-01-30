################################################################################
##
## Loading the binary code
##
################################################################################

## build the DLL name
wink_dll <- paste("wink", .Platform$dynlib.ext, sep="")
print(paste("WINK: Loading",wink_dll) )

## load the dll only once
if( is.loaded("wink_version") ) dyn.unload(wink_dll);
dyn.load(wink_dll)
if( !is.loaded("wink_version") ) stop("WINK: Unable to find wink_version!");

################################################################################
##
## display the version
##
################################################################################
wink_version <- function()
{
    .Call("wink_version");
}

########################################################################
##
## permutation of 1..n
##
## return a random permutation of 1..n as a VECTOR
##
########################################################################
wink_perm <- function(n)
{
    .Call("wink_perm", as.integer(n) );
}

########################################################################
##
## Number of total true coincidences
## N1:        first neuron
## N2:        second neuron
## intervals: matrix, two rows , each colum is [a,b]
## delta:     time lag
## S:         "T" or "H.
##
## return: the VECTOR of true_coincidences for each interval
##
########################################################################
wink_true_coincidences <-function(N1,N2,intervals,delta,S)
{
    if( !is.matrix(N1) )       stop("N1 must be a matrix");
    if( !is.matrix(N2) )       stop("N2 must be a matrix");
    if( !is.matrix(intervals)) stop("intervals must be a matrix");
    if( !is.real(delta) )      stop("delta must be a real");
    if( !is.character(S) )     stop("S must be a string");
    
    ## call the C++ code
    .Call("wink_true_coincidences",N1,N2,intervals,delta,S)
}
