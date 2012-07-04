
dyn.load("wink.so")
source("simulationPP.R")
neurons <- simulPP(0,50,3,10);
windows <- matrix( data=0, nrow = 2, ncol=3 );
windows[1,1]=0;  windows[2,1]=10;
windows[1,2]=10; windows[2,2]=20;
windows[1,3]=20; windows[2,3]=30;

windows;

process <- function(N1,N2,windows,delta,B)
{
	## check arguments in R
	if (!is.matrix(N1))
		stop("N1 must be a matrix")
	if (!is.matrix(N2))
		stop("N2 must be a matrix")
	if( !is.matrix(windows) )
		stop("windows must be a matrix")
	if( !is.real(delta) )
		stop("delta must be a real")
		
	if( !is.real(B) )
		stop("B must be a real/integer")
	
	
	.Call("demo", N1, N2,windows,delta,B );
	#return: the pvalues associated to the windows
}

process(neurons[[1]],neurons[[2]],windows,0.02,100);



