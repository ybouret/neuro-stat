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
NeuroCorr_SetParallel(0);
stamp  = proc.time();
seqAns = NeuroCorr_Compute(DataNeur,Nneur,scale,delta,K,BoxEst,"byKind");
seqTmx = proc.time() - stamp;



print(seqAns);
print(seqTmx);

## Parallel Computation
NeuroCorr_SetParallel(4);
stamp  = proc.time();
parAns = NeuroCorr_Compute(DataNeur,Nneur,scale,delta,K,BoxEst,"byKind");
parTmx = proc.time() - stamp;
print(parTmx);

## check the same
differences=c(
norm(seqAns$mu1$'1' - parAns$mu1$'1',type="f"),
norm(seqAns$mu2$'1' - parAns$mu2$'1',type="f"),
norm(seqAns$muA$'1' - parAns$muA$'1',type="f"),
norm(seqAns$G$'1'   - parAns$G$'1',  type="f")
)
## must be 0...
print(differences);

speedUp = seqTmx[[3]]/parTmx[[3]];
print(paste("speedUp=",speedUp));