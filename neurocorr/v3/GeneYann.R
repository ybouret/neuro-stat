rm(list=ls())

source('SimuHawkesPatou.R')

Nneur=8
Tfin=20
Nessai=1
delai= 1

spontComp=matrix(rep(20,Nneur), nrow=Nneur)

interacComp			= array(c(list()),c(Nneur,Nneur,1))

interacComp[1,1,1][[1]]	= matrix(c(0,0,0.02,0),nrow = 2)
interacComp[1,2,1][[1]]	= matrix(c(0,25,0.02,0),nrow = 2)
interacComp[1,3,1][[1]]	= matrix(c(0,25,0.02,0),nrow = 2)
interacComp[1,4,1][[1]]	= matrix(c(0,0,0.02,0),nrow = 2)
interacComp[1,5,1][[1]]	= matrix(c(0,0,0.02,0),nrow = 2)
interacComp[1,6,1][[1]]	= matrix(c(0,0,0.02,0),nrow = 2)
interacComp[1,7,1][[1]]	= matrix(c(0,0,0.02,0),nrow = 2)
interacComp[1,8,1][[1]]	= matrix(c(0,0,0.02,0),nrow = 2)

interacComp[2,1,1][[1]]	= matrix(c(0,0,0.02,0),nrow = 2)
interacComp[2,2,1][[1]]	= matrix(c(0,25,0.02,0),nrow = 2)
interacComp[2,3,1][[1]]	= matrix(c(0,0,0.02,0),nrow = 2)
interacComp[2,4,1][[1]]	= matrix(c(0,0,0.02,0),nrow = 2)
interacComp[2,5,1][[1]]	= matrix(c(0,0,0.02,0),nrow = 2)
interacComp[2,6,1][[1]]	= matrix(c(0,0,0.02,0),nrow = 2)
interacComp[2,7,1][[1]]	= matrix(c(0,0,0.02,0),nrow = 2)
interacComp[2,8,1][[1]]	= matrix(c(0,0,0.02,0),nrow = 2)

interacComp[3,1,1][[1]]	= matrix(c(0,25,0.02,0),nrow = 2)
interacComp[3,2,1][[1]]	= matrix(c(0,25,0.02,0),nrow = 2)
interacComp[3,3,1][[1]]	= matrix(c(0,0,0.02,0),nrow = 2)
interacComp[3,4,1][[1]]	= matrix(c(0,0,0.02,0),nrow = 2)
interacComp[3,5,1][[1]]	= matrix(c(0,0,0.02,0),nrow = 2)
interacComp[3,6,1][[1]]	= matrix(c(0,0,0.02,0),nrow = 2)
interacComp[3,7,1][[1]]	= matrix(c(0,0,0.02,0),nrow = 2)
interacComp[3,8,1][[1]]	= matrix(c(0,0,0.02,0),nrow = 2)

interacComp[4,1,1][[1]]	= matrix(c(0,0,0.02,0),nrow = 2)
interacComp[4,2,1][[1]]	= matrix(c(0,0,0.02,0),nrow = 2)
interacComp[4,3,1][[1]]	= matrix(c(0,0,0.02,0),nrow = 2)
interacComp[4,4,1][[1]]	= matrix(c(0,0,0.02,0),nrow = 2)
interacComp[4,5,1][[1]]	= matrix(c(0,0,0.02,0),nrow = 2)
interacComp[4,6,1][[1]]	= matrix(c(0,0,0.02,0),nrow = 2)
interacComp[4,7,1][[1]]	= matrix(c(0,0,0.02,0),nrow = 2)
interacComp[4,8,1][[1]]	= matrix(c(0,0,0.02,0),nrow = 2)

interacComp[5,1,1][[1]]	= matrix(c(0,0,0.02,0),nrow = 2)
interacComp[5,2,1][[1]]	= matrix(c(0,0,0.02,0),nrow = 2)
interacComp[5,3,1][[1]]	= matrix(c(0,0,0.02,0),nrow = 2)
interacComp[5,4,1][[1]]	= matrix(c(0,0,0.02,0),nrow = 2)
interacComp[5,5,1][[1]]	= matrix(c(0,0,0.02,0),nrow = 2)
interacComp[5,6,1][[1]]	= matrix(c(0,0,0.02,0),nrow = 2)
interacComp[5,7,1][[1]]	= matrix(c(0,0,0.02,0),nrow = 2)
interacComp[5,8,1][[1]]	= matrix(c(0,25,0.02,0),nrow = 2)

interacComp[6,1,1][[1]]	= matrix(c(0,0,0.02,0),nrow = 2)
interacComp[6,2,1][[1]]	= matrix(c(0,0,0.02,0),nrow = 2)
interacComp[6,3,1][[1]]	= matrix(c(0,0,0.02,0),nrow = 2)
interacComp[6,4,1][[1]]	= matrix(c(0,0,0.02,0),nrow = 2)
interacComp[6,5,1][[1]]	= matrix(c(0,25,0.02,0),nrow = 2)
interacComp[6,6,1][[1]]	= matrix(c(0,0,0.02,0),nrow = 2)
interacComp[6,7,1][[1]]	= matrix(c(0,0,0.02,0),nrow = 2)
interacComp[6,8,1][[1]]	= matrix(c(0,0,0.02,0),nrow = 2)

interacComp[7,1,1][[1]]	= matrix(c(0,0,0.02,0),nrow = 2)
interacComp[7,2,1][[1]]	= matrix(c(0,0,0.02,0),nrow = 2)
interacComp[7,3,1][[1]]	= matrix(c(0,0,0.02,0),nrow = 2)
interacComp[7,4,1][[1]]	= matrix(c(0,0,0.02,0),nrow = 2)
interacComp[7,5,1][[1]]	= matrix(c(0,0,0.02,0),nrow = 2)
interacComp[7,6,1][[1]]	= matrix(c(0,25,0.02,0),nrow = 2)
interacComp[7,7,1][[1]]	= matrix(c(0,0,0.02,0),nrow = 2)
interacComp[7,8,1][[1]]	= matrix(c(0,0,0.02,0),nrow = 2)

interacComp[8,1,1][[1]]	= matrix(c(0,0,0.02,0),nrow = 2)
interacComp[8,2,1][[1]]	= matrix(c(0,0,0.02,0),nrow = 2)
interacComp[8,3,1][[1]]	= matrix(c(0,0,0.02,0),nrow = 2)
interacComp[8,4,1][[1]]	= matrix(c(0,0,0.02,0),nrow = 2)
interacComp[8,5,1][[1]]	= matrix(c(0,0,0.02,0),nrow = 2)
interacComp[8,6,1][[1]]	= matrix(c(0,0,0.02,0),nrow = 2)
interacComp[8,7,1][[1]]	= matrix(c(0,25,0.02,0),nrow = 2)
interacComp[8,8,1][[1]]	= matrix(c(0,0,0.02,0),nrow = 2)

CaseComp			= array(c(list()),c(Nessai,1))

for (nessai in 1:Nessai)
{
CaseComp[[nessai]]	= matrix(c(0,Tfin,1),nrow=3) 
}

DataNeur		= Hawkesmulti(Nessai,delai,CaseComp,spontComp,interacComp)

DataNeur[,1]
l=ncol(DataNeur)

d=diff(DataNeur[1,2:l])
BoxEst=matrix(c(1,0,Tfin,1),nrow=4)
options(digits=12)

