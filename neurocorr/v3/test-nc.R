source("neurocorr.R");


NeuroCorr_Version()


source("gen_data_neur.R");

scale=1e6;

if(FALSE)
{
NeuroCorr_CheckData(DataNeur,Nneur,scale);
Phi0 = NeuroCorr_ComputePhi0(DataNeur,Nneur,scale,0.05);
plot(Phi0$graph,type="l",col="black");
points(Phi0$train,col="red");
}

NeuroCorr_SetParallel(4);

mtx=NeuroCorr_Compute(DataNeur,Nneur,scale,0.05,3,BoxEst,"byKind");

NeuroCorr_CleanUp();
