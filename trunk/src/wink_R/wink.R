########################################################################
##
## loading DLL
##
########################################################################

## build the DLL name
wink_dll <- paste("wink", .Platform$dynlib.ext, sep="")
print(wink_dll)

## load the dll
dyn.load(wink_dll)

########################################################################
##
## check arguments
##
########################################################################
wink_bootstrap_check_args <- function(N1,N2,intervals,delta,B)
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
		
	if( !is.real(B) )
		stop("B must be a real/integer")
}

########################################################################
##
## Define the sequential function
##
########################################################################
bootstrap_pvalues_ser <- function(N1,N2,intervals,delta,B)
{
	# common arguments check
	wink_bootstrap_check_args(N1,N2,intervals,delta,B);
		
	#return: the pvalues associated to each interval
	.Call("wink_ser", N1, N2,intervals,delta,B );
	
}


bootstrap_pvalues_par <- function(N1,N2,intervals,delta,B,num_threads)
{
	
	# common arguments check
	wink_bootstrap_check_args(N1,N2,intervals,delta,B);
	
	# num threads check
	if( !is.real(num_threads) )
		stop("num_threads must be a real/integer")
		
	#return: the pvalues associated to the windows
	.Call("wink_par", N1, N2,intervals,delta,B,num_threads);
}
