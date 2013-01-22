########################################################################
##
## loading DLL
##
########################################################################


## build the DLL name
wink_dll <- paste("wink", .Platform$dynlib.ext, sep="")
print(paste("WINK: Loading",wink_dll) )

## load the dll only once
if( is.loaded("wink_version") )
	dyn.unload(wink_dll)
dyn.load(wink_dll)
if( !is.loaded("wink_version") )
	stop("WINK: Unable to find wink_version!")
	
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
## Check Common arguments, level-0
##
## N1       : first Neuron
## N2       : second Neuron
## intervals: #intervals, 2 rows of a,b
## delta    : time lag for coincidence
##
########################################################################
wink_check_common0 <- function(N1,N2,intervals,delta)
{
	## check arguments in R
	if (!is.matrix(N1))
		stop("N1 must be a matrix")
		
	if (!is.matrix(N2))
		stop("N2 must be a matrix")
		
	if( !is.matrix(intervals) )
		stop("intervals must be a matrix")
		
	if( !is.real(delta) )
		stop("delta must be a real")
}

########################################################################
##
## Number of total true coincidences
## S: "T" or "H.
## return: the VECTOR of true_coincidences for each interval
##
########################################################################
wink_true_coincidences <-function(N1,N2,intervals,delta,S)
{
	wink_check_common0(N1,N2,intervals,delta);
	if( !is.character(S) )
		stop("S is not a string");
	## call the C++ code
	.Call("wink_true_coincidences",N1,N2,intervals,delta,S)
}

########################################################################
##
## Check Common Arguments, level-1
##
## N1       : first Neuron
## N2       : second Neuron
## intervals: #intervals, 2 rows of a,b
## delta    : time lag for coincidence
## B        : bootstraps count
## S        : "T", "H"
########################################################################
wink_check_common1 <- function(N1,N2,intervals,delta,B,S)
{
	wink_check_common0(N1,N2,intervals,delta);
	
	if( !is.real(B) )
		stop("delta must be a real");
		
	if( !is.character(S) )
		stop("S is not a string");
}

wink_permutation <- function(N1,N2,intervals,delta,B,S)
{
	wink_check_common1(N1,N2,intervals,delta,B,S);
	
	.Call("wink_permutation",N1,N2,intervals,delta,B,S);
}


wink_permutation_par <- function(N1,N2,intervals,delta,B,S,num_threads)
{
	wink_check_common1(N1,N2,intervals,delta,B,S);
	
	if( !is.real(num_threads) )
		stop("num_threads is not a real");
		
	.Call("wink_permutation_par",N1,N2,intervals,delta,B,S,num_threads);
}

