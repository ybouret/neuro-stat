source("wink.R");
source("simulationPP.R")

neurons <- simulPP(0,2,50,40);

shift=0.01
ab=0.1
a=seq(0,2-ab,by=shift)
b=a+ab
intervals=t(matrix(c(a,b),ncol=2))

B=1000
delta=0.01

