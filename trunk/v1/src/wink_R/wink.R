########################################################################
##
## loading DLL
##
########################################################################


## build the DLL name
wink_dll <- paste("wink", .Platform$dynlib.ext, sep="")
print(wink_dll)

## load the dll only once
if( is.loaded("wink_ser") )
	dyn.unload(wink_dll)
	
dyn.load(wink_dll)


########################################################################
##
## Test Functions
##
########################################################################

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
##
## N1       : first Neuron
## N2       : second Neuron
## intervals: #intervals, 2 rows of a,b
## delta    : time lag for coincidence
##
## return: the vector of true_coincidences for each interval
##
########################################################################
wink_true_coincidences <-function(N1,N2,intervals,delta)
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
		
	## call the C++ code
	.Call("wink_true_coincidences",N1,N2,intervals,delta)
}


########################################################################
##
## check arguments for bootstrapping functions
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
## N1       : first Neuron
## N2       : second Neuron
## intervals: #intervals, 2 rows of a,b
## delta    : time lag for coincidence
## B        : bootstrap count
##
## return: the vector of pvalues(Bootstrap>=true_coincidences)
########################################################################
bootstrap_pvalues_ser <- function(N1,N2,intervals,delta,B)
{
	# common arguments check
	wink_bootstrap_check_args(N1,N2,intervals,delta,B);
		
	#return: the pvalues associated to each interval
	.Call("wink_ser", N1, N2,intervals,delta,B );
	
}

########################################################################
##
## Define the sequential function
##
## N1       : first Neuron
## N2       : second Neuron
## intervals: #intervals, 2 rows of a,b
## delta    : time lag for coincidence
## B        : bootstrap count
##
## return   : a MATRIX: first  row: pvalues(Bootstrap>=true_coincidences)
##                      second row: pvalues(Bootstrap<=true_coincidences)
########################################################################
bootstrap_both_pvalues_ser <- function(N1,N2,intervals,delta,B)
{
	# common arguments check
	wink_bootstrap_check_args(N1,N2,intervals,delta,B);
		
	#return: the pvalues associated to each interval
	.Call("wink_both_ser", N1, N2,intervals,delta,B );
	
}





########################################################################
##
## Define the parallel function
##
## N1       : first Neuron
## N2       : second Neuron
## intervals: #intervals, 2 rows of a,b
## delta    : time lag for coincidence
## B        : bootstrap count
## num_threads: number of threads to dispatch intervals
##
## return: the vector of pvalues(Bootstrap>=true_coincidences)
########################################################################
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

########################################################################
##
## Define the parallel function
##
## N1       : first Neuron
## N2       : second Neuron
## intervals: #intervals, 2 rows of a,b
## delta    : time lag for coincidence
## B        : bootstrap count
## num_threads: number of threads to dispatch intervals
##
## return   : a MATRIX: first  row: pvalues(Bootstrap>=true_coincidences)
##                      second row: pvalues(Bootstrap<=true_coincidences)
########################################################################
bootstrap_both_pvalues_par <- function(N1,N2,intervals,delta,B,num_threads)
{
	
	# common arguments check
	wink_bootstrap_check_args(N1,N2,intervals,delta,B);
	
	# num threads check
	if( !is.real(num_threads) )
		stop("num_threads must be a real/integer")
		
	#return: the pvalues associated to the windows
	.Call("wink_both_par", N1, N2,intervals,delta,B,num_threads);
}
