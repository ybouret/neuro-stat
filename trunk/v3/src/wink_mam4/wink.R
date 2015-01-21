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

#wink_xts_dll <- paste("wink_xts",.Platform$dynlib.ext, sep="");
#if( is.loaded("wink_xts") ) dyn.unload(wink_xts_dll);
#dyn.load(wink_xts_dll);

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
wink_true_coincidences <-function(S,N1,N2,intervals,delta)
{
    if( !is.character(S) )     stop("S must be a string");
    if( !is.matrix(N1) )       stop("N1 must be a matrix");
    if( !is.matrix(N2) )       stop("N2 must be a matrix");
    if( !is.matrix(intervals)) stop("intervals must be a matrix");
    if( !is.double(delta) )    stop("delta must be a real");
    
    ## call the C++ code
    .Call("wink_true_coincidences",S,N1,N2,intervals,delta)
}

########################################################################
##
## Evaluate permutation pvalues
##
## S:          "T" or "H.
## N1:         first neuron
## N2:         second neuron
## intervals:  matrix, two rows , each colum is [a,b]
## delta:      time lag
## B:          number of permutations
## num_threads: 1 --> serial code, >1 --> parallel code
##
## return: the MATRIX with row[1]= pvalues^-
##                         row[2]= pvalues^+
##
########################################################################
wink_permutation_pvalues <- function(S,N1,N2,intervals,delta,B,num_threads)
{
    if( !is.character(S) )       stop("S must be a string");
    if( !is.matrix(N1) )         stop("N1 must be a matrix");
    if( !is.matrix(N2) )         stop("N2 must be a matrix");
    if( !is.matrix(intervals))   stop("intervals must be a matrix");
    if( !is.double(delta) )      stop("delta must be a real");
    if( !is.double(B) )          stop("B must be a real");
    if( !is.double(num_threads)) stop("num_threads must be a real");
    .Call("wink_permutation_pvalues",S,N1,N2,intervals,delta,B,num_threads);
}

########################################################################
##
## Evaluate bootstrap counts
##
## N1:        first neuron
## N2:        second neuron
## intervals: matrix, two rows , each colum is [a,b]
## delta:     time lag
## B:         number of bootstraps
## num_threads: 1 --> serial code, >1 --> parallel code
## mix_name:    "" --> Pn, "tau" --> tau(Pn), "TS" --> Trial Shuffling
##
## return: the MATRIX with row[1]= counts^-
##                         row[2]= counts^+
##
########################################################################
wink_bootstrap_counts <- function(N1,N2,intervals,delta,B,num_threads,mix_name="")
{
    if( !is.matrix(N1) )         stop("N1 must be a matrix");
    if( !is.matrix(N2) )         stop("N2 must be a matrix");
    if( !is.matrix(intervals))   stop("intervals must be a matrix");
    if( !is.double(delta) )      stop("delta must be a real");
    if( !is.double(B) )          stop("B must be a real");
    if( !is.double(num_threads)) stop("num_threads must be a real");
    if( !is.character(mix_name)) stop("mix must be a string");
    .Call("wink_bootstrap_counts",N1,N2,intervals,delta,B,num_threads,mix_name);
}

########################################################################
##
## Evaluate bootstrap counts by Trial Shuffling
##
## N1:        first neuron
## N2:        second neuron
## intervals: matrix, two rows , each colum is [a,b]
## delta:     time lag
## B:         number of bootstraps
## num_threads: 1 --> serial code, >1 --> parallel code
##
## return: the MATRIX with row[1]= counts^-
##                         row[2]= counts^+
##
## REMARK: the drawing is always "TS"
##
########################################################################
wink_TS_counts <- function(N1,N2,intervals,delta,B,num_threads)
{
    if( !is.matrix(N1) )         stop("N1 must be a matrix");
    if( !is.matrix(N2) )         stop("N2 must be a matrix");
    if( !is.matrix(intervals))   stop("intervals must be a matrix");
    if( !is.double(delta) )      stop("delta must be a real");
    if( !is.double(B) )          stop("B must be a real");
    if( !is.double(num_threads)) stop("num_threads must be a real");
    #if( !is.character(mix_name)) stop("mix must be a string");
    .Call("wink_TS_counts",N1,N2,intervals,delta,B,num_threads);
}


########################################################################
##
## return a list L with: L$S = S(N1,N2) and L$Sp = S^*(N1,N2) (SORTED)
## for B iterations and S="T" or S="H"
##
##
## S    : "T" or "H:
## N1   :first Neuron
## N2   : second Neuron
## a    : window start
## b    : window end
## delta: time lag
## B    : number of iterations
##
########################################################################
wink_single_perm <- function(S,N1,N2,a,b,delta,B)
{
    if( !is.matrix(N1) )       stop("N1 must be a matrix");
    if( !is.matrix(N2) )       stop("N2 must be a matrix");
    if( !is.double(delta) )    stop("delta must be a real");
    if( !is.double(a) )        stop("delta must be a real");
    if( !is.double(b) )        stop("delta must be a real");
    if( !is.double(B) )        stop("B must be a real");
    .Call("wink_single_perm",S,N1,N2,a,b,delta,B);
}

########################################################################
##
## return a list L with: L$H = H(N1,N2) and L$Hc = H^*(N1,N2) (SORTED)
## for B bootstrap
##
## see test_single.R for more info
##
## N1   :first Neuron
## N2   : second Neuron
## a    : window start
## b    : window end
## delta: time lag
## B    : bootstrap count
## mix_name: "" --> Pn, "tau" --> tau(Pn), "TS" --> Trial Shuffling
##
########################################################################
wink_single_boot <- function(N1,N2,a,b,delta,B,mix_name="")
{
    if (!is.matrix(N1))    stop("N1 must be a matrix");
    if (!is.matrix(N2))    stop("N2 must be a matrix");
    if( !is.double(a) )    stop("a must be a real");
    if( !is.double(b) )    stop("b must be a real");
    if( !is.double(delta)) stop("delta must be a real");
    if( !is.double(B) )    stop("B must be a real");
    .Call("wink_single_boot",N1,N2,a,b,delta,B,mix_name);
}

########################################################################
##
## return a list L with: L$S = S(N1,N2) and L$Sts = S^*(N1,N2) (SORTED)
## and L$centering = the_centering_value 175 190 189 178 185 179 176 185 183 199 199 189 187 197 210 208 219 216
## [19] 2
## for B iterations and S="T" or S="H"
##
##
## S    : "T" or "H:
## N1   :first Neuron
## N2   : second Neuron
## a    : window start
## b    : window end
## delta: time lag
## B    : number of iterations
##
########################################################################
wink_single_TS <- function(S,N1,N2,a,b,delta,B)
{
    if( !is.matrix(N1) )       stop("N1 must be a matrix");
    if( !is.matrix(N2) )       stop("N2 must be a matrix");
    if( !is.double(delta) )    stop("delta must be a real");
    if( !is.double(a) )        stop("a must be a real");
    if( !is.double(b) )        stop("b must be a real");
    if( !is.double(B) )        stop("B must be a real");
    .Call("wink_single_TS",S,N1,N2,a,b,delta,B);
}

########################################################################
##
## return the coincidence matrix
##
########################################################################
wink_coincmat <- function(N1,N2,a,b,delta)
{
    if( !is.matrix(N1) )       stop("N1 must be a matrix");
    if( !is.matrix(N2) )       stop("N2 must be a matrix");
    if( !is.double(delta) )    stop("delta must be a real");
    if( !is.double(a) )        stop("a must be a real");
    if( !is.double(b) )        stop("b must be a real");
    .Call("wink_coincmat",N1,N2,a,b,delta);
}

wink_save_coincmat <- function(CM,filename)
{
    if( !is.matrix(CM) )         stop("CM must be a matrix");
    if( !is.character(filename)) stop("filename must be a string");
    .Call("wink_save_coincmat",CM,filename);
}


##########CODE PERSO###################################################

#######################################################################
##
## Return the estimator of the firing rate
##
#######################################################################

lambdaChapeau <- function(a,b,N){
	if( !is.matrix(N) )       stop("N must be a matrix");
	if( !is.double(a) )        stop("a must be a real");
	if( !is.double(b) )        stop("b must be a real");
	#.Call("lambdaChapeau",a,b,N);
	.Call("lambdaChapeau1",a,b,N);
}


#######################################################################
##
## Return the estimator of the espectation 
##
#######################################################################

m0Chapeau <- function(a,b,delta,N1,N2){
	if( !is.matrix(N1) )       stop("N1 must be a matrix");
	if( !is.matrix(N2) )       stop("N2 must be a matrix");
	if( !is.double(a) )        stop("a must be a real");
	if( !is.double(b) )        stop("b must be a real");
	if( !is.double(delta) )    stop("delta must be a real");
	#.Call("lambdaChapeau",a,b,N);
	.Call("m0Chapeau1",a,b,delta,N1,N2);
}

#######################################################################
##
## Return the p-value
##
## testName to choose one of the three tests : 
##	"symetric" for the symetrical test
##	"upper"    for the unilateral upper value test
##	"lower"    for the unilateral lower value test
##
##
#######################################################################


pValue <- function (a,b,delta,N1,N2,testName){
	if( !is.matrix(N1) )       stop("N1 must be a matrix");
	if( !is.matrix(N2) )       stop("N2 must be a matrix");
	if( !is.double(delta) )    stop("delta must be a real");
	if( !is.double(a) )        stop("a must be a real");
	if( !is.double(b) )        stop("b must be a real");
	if( !is.character(testName) )        stop("testName must be a string");
	if( testName!="symetric" && testName!="upper" && testName!="lower")        stop("testName must be \"symetric\" or \"upper\" or \"lower\"");

	if ( testName=="symetric" ) {   .Call("pValue1",a,b,delta,N1,N2,1);}
	else if ( testName=="upper" ){    .Call("pValue1",a,b,delta,N1,N2,2);}
	else{    .Call("pValue1",a,b,delta,N1,N2,3);}

}

pValues <- function (intervals,delta,N1,N2,testName){
	if( !is.matrix(N1) )       stop("N1 must be a matrix");
	if( !is.matrix(N2) )       stop("N2 must be a matrix");
	if( !is.double(delta) )    stop("delta must be a real");
	if( !is.matrix(intervals) )        stop("M must be a Matrix");
	if( !is.character(testName) )        stop("testName must be a string");
	if( testName!="symetric" && testName!="upper" && testName!="lower")        stop("testName must be \"symetric\" or \"upper\" or \"lower\"");
	if ( testName=="symetric" ) {   .Call("pValues",intervals,delta,N1,N2,1);}
	else if ( testName=="upper" ){    .Call("pValues",intervals,delta,N1,N2,2);}
	else{    .Call("pValues",intervals,delta,N1,N2,3);}
}


pValuesPara <- function (intervals,delta,N1,N2,testName,num_threads)
{
    if( !is.matrix(N1) )          stop("N1 must be a matrix");
    if( !is.matrix(N2) )          stop("N2 must be a matrix");
    if( !is.double(delta) )       stop("delta must be a real");
    if( !is.matrix(intervals) )   stop("M must be a Matrix");
    if( !is.character(testName) ) stop("testName must be a string");
    if( !is.double(num_threads) ) stop("num_threads must be a number");

    if( testName!="symetric" && testName!="upper" && testName!="lower")        stop("testName must be \"symetric\" or \"upper\" or \"lower\"");
    if ( testName=="symetric" )  {   .Call("pValuesPara",intervals,delta,N1,N2,1,num_threads);}
    else if ( testName=="upper" ){   .Call("pValuesPara",intervals,delta,N1,N2,2,num_threads);}
    else{                            .Call("pValuesPara",intervals,delta,N1,N2,3,num_threads);}
}












