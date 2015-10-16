DataNeur=matrix(scan("n8t1end20.txt"),nrow=8);
Tfin=20;
Tfin=1;
BoxEst=matrix(c(1,0,Tfin,1),nrow=4)

source("neurocorr.R");

Nneur=8;
scale=1e6;
delta=0.005;
K=20;

stamp = proc.time();
seq_mtx=NeuroCorr_Compute(DataNeur,Nneur,scale,delta,K,BoxEst,"byKind");
seq_tmx = proc.time() - stamp;

NeuroCorr_SetParallel(4);

stamp = proc.time();
par_mtx=NeuroCorr_Compute(DataNeur,Nneur,scale,delta,K,BoxEst,"byKind");
par_tmx = proc.time() - stamp;

## check the same
differences=c(
norm(seq_mtx$mu1$'1' - par_mtx$mu1$'1',type="f"),
norm(seq_mtx$mu2$'1' - par_mtx$mu2$'1',type="f"),
norm(seq_mtx$muA$'1' - par_mtx$muA$'1',type="f"),
norm(seq_mtx$G$'1'   - par_mtx$G$'1',  type="f")
)
## must be 0...
print(differences);

speedUp = seq_tmx[[3]]/par_tmx[[3]];
print(paste("speedUp=",speedUp));

## todo before leaving R...
NeuroCorr_CleanUp();
