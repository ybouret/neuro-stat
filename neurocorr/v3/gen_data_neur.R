# This program is  just a test :
# - it generates the data , depending on the box, the behavior may change. In particular several boxes may have the same behavior, for the moment there is only one trial
# - it applies the Lasso procedure to produce the estimate and plot it

#rm(list=ls()) # clear everything

# programs for the simulations

source('../codes/SimuHawkes.R',chdir=TRUE)

# package for Lasso
#library(lassoshooting)

# programs for the computation of the least-square contrast

#source('Nij.R')
#source('prodN.R')
#source('produit.R')
#source('bnc.R')
##source('vb.R')
#source('Xtc.R')
#source('Xc.R')
#source('intijnc.R')
#source('fintegrale.R')

# programs for the Lasso estimate
#source('BoxLassoHawkes.R')
#source('BLHPlotbis.R')

########################################################################################################
#																									   #
# 										SIMULATION													   #
#																									   #
########################################################################################################

Nneur=2  # number of neurons
Ntrial=7 # number of trials
Ncomp=2 # number of comportments, for the moment just one relaxation state and one exciting state

delay=0 # time in second during which the process warms up
Tend = 100 # duration in second of the trial, each process is simulated on [0,Tend], here for this simulation should be a multiple of 10
Tend = 10;

### spontaneous apparition rate in each comportment/behavior : line =neuron column = comportment
spontComp=matrix(c(10,10,10,10), nrow=2)


### Interaction functions per comportment/behavior : first index = influenced neuron, second index = influencing neuron, third index = comportment. Do not touch to [[1]], it is just a way to put arrays of different size inside another one. Each function is coded by a matrix : first line the partition, second line the value on the partition. The partition needs to be in [0,something>0] so at least two values even if the function is 0.  The second line needs to end with a 0, ie no interaction after a certain range. The second line may have negative values but the spectral radius of [\int max(h_l^m,0)]_{l,m} needs to be strictly smaller than 1 or the simulation may never end.

interacComp			= array(c(list()),c(Nneur,Nneur,Ncomp))
interacComp[1,1,1][[1]]	= matrix(c(0,0,0.02,0),nrow = 2)
interacComp[1,2,1][[1]]	= matrix(c(0,0,0.02,0),nrow = 2)
interacComp[2,1,1][[1]]	= matrix(c(0,0,0.02,0),nrow = 2)
interacComp[2,2,1][[1]]	= matrix(c(0,0,0.02,0),nrow = 2)

interacComp[1,1,2][[1]]	= matrix(c(0,30,0.02,0),nrow = 2)
interacComp[1,2,2][[1]]	= matrix(c(0,30,0.01,0,0.02,0),nrow = 2)
interacComp[2,1,2][[1]]	= matrix(c(0,0,0.01,30,0.02,0),nrow = 2)
interacComp[2,2,2][[1]]	= matrix(c(0,0,0.02,0),nrow = 2)


### repartition of the behaviors for each trial, each cell corresponds to a trial, in each cell a matrix with three lines : first beginning of the box, second end of the box, third comportment  number

BoxComp			= array(c(list()),c(Ntrial,1))

a=sort(c(seq(0,Tend,by=10),seq(1,Tend-9,by=10)))
b=length(a)-1
x=matrix(c(a[1],rep(a[2:b],each=2),a[length(a)]),nrow=2)
y=rep(c(2,1),Tend/10)
boxes=t(matrix(c(t(x),y), ncol=3)) ### here 2 comportments, 2 =excited during 1 s then 9 s of relaxation then again same thing

for (n in 1:Ntrial)
{
    BoxComp[[n]]	= boxes
}

### simulation of the data: DataNeur is a matrix.
###each line corresponds to 1 trial for 1 neuron, first column = number of points of 1 trial for 1 neuron, other columns = the (strictly positive) points then 0 to complete the line
### in line first all the trials for the first neuron then all the trials for the second etc


#DataNeur		= Hawkesmulti(Ntrial,delay,BoxComp,spontComp,interacComp)
BoxEst=t(matrix(c(rep(1,2*Tend/10),t(boxes)),ncol=4))

