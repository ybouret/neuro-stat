DataNeur=matrix(scan("n8t1end20.txt"),nrow=8);
Tfin=20;
#Tfin=1;
BoxEst=matrix(c(1,0,Tfin,1),nrow=4)

source("neurocorr.R");

Nneur=8;
scale=1e6;

delta=0.005;
K=4;

## Sequential Computation
NeuroCorr_SetParallel(4);

ans = NeuroCorr_Compute(DataNeur,Nneur,scale,delta,K,BoxEst,"byKind");

G   = ans$G[[1]];
mu1 = ans$mu1[[1]];
mu2 = ans$mu2[[1]];
muA = ans$muA[[1]];

gam = 1.1;

coeff=NeuroCorr_Coeff(G,mu1,mu2,muA,gam);
print(coeff);
