source("neurocorr.R");


NeuroCorr_Version()


source("gen_data_neur.R");

scale=1e6;
NeuroCorr_CheckData(DataNeur,Nneur,scale);
Phi0 = NeuroCorr_ComputePhi0(DataNeur,Nneur,scale,0.05);
