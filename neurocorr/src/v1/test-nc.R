################################################################################
##
## simple tests
##
################################################################################

source("neurocorr.R");

source("gen_data_neur.R");

DataNeur		= Hawkesmulti(Ntrial,delay,BoxComp,spontComp,interacComp);
print(DataNeur);
