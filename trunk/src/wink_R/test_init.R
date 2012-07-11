source("wink.R");
source("simulationPP.R")

neurons <- simulPP(0,2,50,40);

ab=0.01
a=seq(0,2-ab,by=ab)
b=a+0.1
intervals=t(matrix(c(a,b),ncol=2))

B=10000
delta=0.01

