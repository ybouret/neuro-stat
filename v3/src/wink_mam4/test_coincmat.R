source("wink.R");
source("simulationPP.R")

neurons <- simulPP(0,2,50,10);

#shift=0.01
#ab=0.1
#a=seq(0,2-ab,by=shift)
#b=a+ab
#intervals=t(matrix(c(a,b),ncol=2))

#B=5000
delta=0.005

CM <- wink_coincmat(neurons[[1]],neurons[[2]],0,0.07,delta);
wink_save_coincmat(CM,"cm.dat");

