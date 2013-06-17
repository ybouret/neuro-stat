rm(list=ls())
source('wink.R')
source('simulationPPindep.R')
source('comparaison_lois.R')

a=0.1
b=0.2
delta=0.01

lambda=30
nessai=20
intervals=t(matrix(c(a,b),ncol=2))

PP="PPH"
B=1000

	# on se fixe une donnee :
	data = simulPPH(a,b,lambda,nessai)

	# echantillon bootstrappe en cassant les couples

	BootauH=wink_single_boot(data[[1]],data[[2]],a,b,delta,B,"tau")
	EchHBtau =BootauH$Hc     # - BootauH$H

	EchP=wink_single_perm("H",data[[1]],data[[2]],a,b,delta,B)
	EchSp=EchP$Sp
	
	
	
	
	
	
	Hb=matrix(data=0,nrow=1,ncol=B)
	for(bb in 1:B)
	{
		m1=sample(1:nessai,nessai,replace=T)
		m2=sample(1:nessai,nessai,replace=T)
		Hbb=wink_true_coincidences("H",data[[1]][m1,],data[[2]][m2,],t(matrix(c(a,b),ncol=2)),delta)
		Hb[bb]=Hbb
	}

	
	xmin=min(EchHBtau,EchSp,Hb)
	xmax=max(EchHBtau,EchSp,Hb)
	ex = (xmax-xmin)
	times = seq(xmin-ex/20,xmax+ex/20,ex/100)

	FRHBtau = FctRep(EchHBtau,times)
	FRHPerm = FctRep(EchSp,times)
	FRHb = FctRep(Hb,times)

	
	# Graphiques	
	plot(times, FRHBtau,xlim=c(xmin,xmax),ylim=c(0,1),col='turquoise',type='s',xlab='',ylab='')
	par(new=TRUE)
	plot(times,FRHPerm,xlim=c(xmin,xmax),ylim=c(0,1),col='orange',type='s',xlab='',ylab='')
	par(new=TRUE)
plot(times,FRHb,xlim=c(xmin,xmax),ylim=c(0,1),col='black',type='s',xlab='',ylab='')

legend(x=xmin,y=1,legend=c("non centered H on bootstrap sample ~ tau(Pn)","H on permuted sample","tau(Pn) version R"),lwd=1,col=c("turquoise","orange","black"),bty="n",ncol=1)

# Chacune des moyennes doivent etre le plus proche de zero possible (reference : permutation)
mean(EchHBtau)
mean(Hb)
mean(EchSp)
