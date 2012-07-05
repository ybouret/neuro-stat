
dyn.load("wink_ser.so")
dyn.load("wink_par.so");

source("simulationPP.R")
neurons <- simulPP(0,2,50,40);

#windows <- matrix( data=0, nrow = 2, ncol=3 );
#windows[1,1]=0;  windows[2,1]=0.1;
#windows[1,2]=0.1; windows[2,2]=0.2;
#windows[1,3]=0.2; windows[2,3]=0.3;

a=seq(0,1.9,by=0.005)
b=a+0.1
windows=t(matrix(c(a,b),ncol=2))

bootstrap_pvalue_ser <- function(N1,N2,windows,delta,B)
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
	
	
	.Call("wink_ser", N1, N2,windows,delta,B );
	#return: the pvalues associated to the windows
}


bootstrap_pvalue_par <- function(N1,N2,windows,delta,B,num_threads)
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
	
	if( !is.real(num_threads) )
		stop("num_threads must be a real/integer")
		
	.Call("wink_par", N1, N2,windows,delta,B,num_threads);
	#return: the pvalues associated to the windows
}

stamp = proc.time();
bootstrap_pvalue_ser(neurons[[1]],neurons[[2]],windows,0.01,10000);
ser_tmx = proc.time() - stamp;
ser_tmx;

stamp = proc.time();
bootstrap_pvalue_par(neurons[[1]],neurons[[2]],windows,0.01,10000,4);
par_tmx = proc.time() - stamp;
par_tmx;


