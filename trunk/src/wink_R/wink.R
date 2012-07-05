
dyn.load("wink.so")
source("simulationPP.R")
neurons <- simulPP(0,2,50,40);

#windows <- matrix( data=0, nrow = 2, ncol=3 );
#windows[1,1]=0;  windows[2,1]=0.1;
#windows[1,2]=0.1; windows[2,2]=0.2;
#windows[1,3]=0.2; windows[2,3]=0.3;

a=seq(0,1.9,by=0.01)
b=a+0.1
windows=t(matrix(c(a,b),ncol=2))

bootstrap_pvalue <- function(N1,N2,windows,delta,B)
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

bootstrap_pvalue(neurons[[1]],neurons[[2]],windows,0.01,10000);



