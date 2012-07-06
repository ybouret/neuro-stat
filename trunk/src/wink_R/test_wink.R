


source("load_wink.R");
source("simulationPP.R")

neurons <- simulPP(0,2,50,40);

#windows <- matrix( data=0, nrow = 2, ncol=3 );
#windows[1,1]=0;  windows[2,1]=0.1;
#windows[1,2]=0.1; windows[2,2]=0.2;
#windows[1,3]=0.2; windows[2,3]=0.3;

a=seq(0,1.9,by=0.01)
b=a+0.1
intervals=t(matrix(c(a,b),ncol=2))


stamp = proc.time();
bootstrap_pvalues_ser(neurons[[1]],neurons[[2]],intervals,0.01,10000);
ser_tmx = proc.time() - stamp;
ser_tmx[3];

stamp = proc.time();
bootstrap_pvalues_par(neurons[[1]],neurons[[2]],intervals,0.01,10000,4);
par_tmx = proc.time() - stamp;
par_tmx[3];

print("-------- SPEEDUP --------");
ser_tmx[[3]]/par_tmx[[3]];

