################################################################################
##
## simple tests
##
################################################################################

source("neurocorr.R");

source("gen_data_neur.R");

DataNeur		= Hawkesmulti(Ntrial,delay,BoxComp,spontComp,interacComp);

print(DataNeur);
print(Nneur);
ND_rows = nrow(DataNeur);
print(ND_rows);
print(ncol(DataNeur));
print(' ');

for(i in 1:ND_rows)
{
    print(DataNeur[i,1]);
}

NeuroCorr_CheckNeuroData(DataNeur,Nneur);
