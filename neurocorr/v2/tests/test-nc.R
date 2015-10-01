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


phi0 = NeuroCorr_ComputePhi(DataNeur,Nneur,scale,0.1,2);



NeuroCorr_CleanUp();
