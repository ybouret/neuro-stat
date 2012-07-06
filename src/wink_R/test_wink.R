


source("wink.R");
source("simulationPP.R")

neurons <- simulPP(0,2,50,40);


a=seq(0,1.9,by=0.005)
b=a+0.1
intervals=t(matrix(c(a,b),ncol=2))

B=10000
delta=0.01

stamp = proc.time();
p0 <- bootstrap_pvalues_ser(neurons[[1]],neurons[[2]],intervals,delta,B);
ser_tmx = proc.time() - stamp;
ser_tmx[3];

stamp = proc.time();
p1 <- bootstrap_pvalues_par(neurons[[1]],neurons[[2]],intervals,delta,B,4);
par_tmx = proc.time() - stamp;
par_tmx[3];

print("-------- SPEEDUP --------");
print(ser_tmx[[3]]/par_tmx[[3]]);

