DataNeur=matrix(scan("n8t1end20.txt"),nrow=8);
Tfin=20;
BoxEst=matrix(c(1,0,Tfin,1),nrow=4)

source("neurocorr.R");

Nneur=8;
scale=1e6;

NeuroCorr_CheckData(DataNeur,Nneur,scale);

delta=0.005;
K=20;