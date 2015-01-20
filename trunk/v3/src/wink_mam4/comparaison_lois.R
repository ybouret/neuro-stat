source('wink.R')


# -------------------------------------------------------------------------------- #
# Creation de ordonnees pour une fonction de repartition empirique (utile pour le moyennage des donnees) 
# -------------------------------------------------------------------------------- #

FctRep <- function(X,T)
{	N=length(X)
	F=c()
	for (t in T)
	{	w=which(X<=t)
		l=length(w)
		F=c(F,l/N)
	}
	return(F)
}


# -------------------------------------------------------------------------------- #
# Echantillon de la vraie loi de la statistique S
# Stat = "T" ou "H" suivant la statistique qu'on veut utiliser
# M = taille de l'echantillon
# PP = "PPH" ou "PPI"
# -------------------------------------------------------------------------------- #

EchStat <- function(Stat,M,PP,a,b,lambda,nessai,delta)
{
	if( (PP!="PPH") & (PP!="PPI") )
	{
		print("PP must be equal to 'PPH' or 'PPI")
	}
	
	if( PP == "PPH")
	{
		EchS=c()
		for(k in 1:M)
		{
			simu=simulPPH(a,b,lambda,nessai)
			Ssimu=wink_true_coincidences(Stat,simu[[1]],simu[[2]],matrix(c(a,b),nrow=2),delta)
			EchS=c(EchS,Ssimu)
		}	
	}
	
	if( PP == "PPI")
	{
		d=b-a
		Temps=c(a,a+d/5,a+2*d/5,a+4*d/5,b)
		Sauts=c(0,lambda,2*lambda,0)
		taux=0.21
		EchS=c()
		for(k in 1:M)
		{
			simu=simulPPI(Temps,Sauts,taux,nessai)
			Ssimu=wink_true_coincidences(Stat,simu[[1]],simu[[2]],matrix(c(a,b),nrow=2),delta)
			EchS=c(EchS,Ssimu)
		}	

	}
	
	return(EchS)
}


# -------------------------------------------------------------------------------- #
# Echantillon permute conditionnel a une donnee
# -------------------------------------------------------------------------------- #

EchPerm <- function(Stat,data,a,b,delta,B)
{
	EchP=wink_single_perm(Stat,data[[1]],data[[2]],a,b,delta,B)
	EchSp=EchP$Sp
	return(EchSp)
}


# -------------------------------------------------------------------------------- #
# Echantillon bootstrap sans casser les couples conditionnel a une donnee
# -------------------------------------------------------------------------------- #

EchBoot <- function(data,a,b,delta,B,mix_boot="")
{	
	EchB=wink_single_boot(data[[1]],data[[2]],a,b,delta,B,mix_boot)
	EchHc=EchB$Hc
	return(EchHc)
}

