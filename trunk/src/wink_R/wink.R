
dyn.load("wink.so")
source("simulationPP.R")
neurons <- simulPP(0,5,1,5);
neurons[[1]]

process <- function(N1,N2)
{
	if (!is.matrix(N1))
		stop("N1 must be a matrix")
	if (!is.matrix(N2))
		stop("N2 must be a matrix")	
	.Call("demo", N1, N2 );
}

process(neurons[[1]],neurons[[2]]);



