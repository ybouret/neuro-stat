source("neurocorr.R");

train=c(1.1,2,2.2);

scale=10;
delta=1.0;

phi = NeuroCorr_Phi0(train,scale,delta);

print(phi);
