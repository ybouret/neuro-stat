################################################################################
##
## simple tests
##
################################################################################

source("../neurocorr.R",chdir=TRUE);

source("gen_data_neur.R");

DataNeur		= Hawkesmulti(Ntrial,delay,BoxComp,spontComp,interacComp);

#print(DataNeur);
print(Nneur);
ND_rows = nrow(DataNeur);
print(ND_rows);
print(ncol(DataNeur));
print(' ');

for(i in 1:ND_rows)
{
    print(DataNeur[i,1]);
}

scale = 1.0e6; # microseconds
NeuroCorr_CheckNeuroData(DataNeur,Nneur,scale);


NeuroCorr_SetParallel(0);
delta=0.005;
K=3;

Sseq = NeuroCorr_ComputePhi(DataNeur,Nneur,scale,delta,K);
plot(Sseq$phi0,type="l");
points(Sseq$spikes,col="red");


NeuroCorr_SetParallel(4);
Spar = NeuroCorr_ComputePhi(DataNeur,Nneur,scale,delta,K);
plot(Spar$phi0,type="l");
points(Spar$spikes,col="red");

NeuroCorr_SetParallel(0);

NeuroCorr_Compute(DataNeur,Nneur,scale,delta,K,BoxEst,"byBox");


NeuroCorr_CleanUp();
