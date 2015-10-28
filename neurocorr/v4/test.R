DataNeur=matrix(scan("n8t1end20.txt"),nrow=8);
Tfin=20;
Tfin=1;
BoxEst=matrix(c(1,0,Tfin,1),nrow=4)

source("neurocorr.R");

Nneur=8;
scale=1e6;

delta=0.005;
K=2;

NeuroCorr_SetParallel(0);
ans=NeuroCorr_Compute(DataNeur,Nneur,scale,delta,K,BoxEst,"byKind");
print(ans);