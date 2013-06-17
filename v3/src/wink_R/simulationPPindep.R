# Sommaire :
# 	- simulation des processus de Poisson homogenes independants pour les donnees :
#		- soit N1 et N2 ont la meme intensite,
#		- soit N1 et N2 n'ont pas la meme intensite
#	- simulation des processus de Poisson inhomogenes independants par la methode du thining

# ------------------------------------------------------------------------------------- #
# Simulation d'un processus de Poisson homogene d'intensite lambda
# ------------------------------------------------------------------------------------- #

# Remarque : Conditionnellement au nb de pts du processus, les tops suivent une loi uniforme sur [a,b] et sont independants

# cas ou on a nneur neurones ayant tous le meme parametre :

simulPPH <- function(a,b,lambda,nessai)
{	p=lambda*(b-a)		# parametre de la loi de Poisson N(b)-N(a)
	N=2*nessai 		# nombre total de donnees a avoir
	ntops=rpois(N,p)	# nombre de tops pour chaque processus
	s=max(ntops)
	M=matrix(data=0,nrow=N,ncol=s+1)
	M[,1]=ntops		# la premiere colonne de M contient le nombre de tops
	for (i in 1:N)		# pour chaque neurone et chaque essai on simule 
	{	r=ntops[i]	# conditionnellement a ntops
		if (r!=0)
		{	x=runif(r,a,b)		# on simule les r uniformes correspondantes
			M[i,2:(r+1)]=sort(x)	# on les re-ordonne
		}
	}
	simu=list(M[1:nessai,],M[(nessai+1):(2*nessai),])
	return(simu)
}

# ------------------------------------------------------------------------------------- #

# cas de 2 neurones ayant des parametres differents :

simulationPP <- function(a,b,lambda1,lambda2,nessai)
{	p1=lambda1*(b-a)
	p2=lambda2*(b-a)
	ntops1=rpois(nessai,p1)
	ntops2=rpois(nessai,p2)
	s=max(c(ntops1,ntops2))
	M1=matrix(data=0,nrow=nessai,ncol=s+1)
	M2=matrix(data=0,nrow=nessai,ncol=s+1)
	M1[,1]=ntops1
	M2[,1]=ntops2
	for (i in 1:nessai)
	{	r1=ntops1[i]
		r2=ntops2[i]
		if (r1!=0)
		{	x=runif(r1,a,b)
			M1[i,2:(r1+1)]=sort(x)
		}
		if (r2!=0)
		{	x=runif(r2,a,b)
			M2[i,2:(r2+1)]=sort(x)
		}
	}
	return(list(M1,M2))
}


# ------------------------------------------------------------------------------------- #
# Simulation d'un processus de Poisson inhomogene d'intensite intensite
# T = (a,t1,t2,...,tk,b) contient les absisces des plateux de l'intensite
# S = (0,s1,s2,...,s(k-1),0) contient les hauteurs des k+1 plateaux
# taux (in [0,1]) regle la hauteur des bosses 
# nR est le nombre de pas dans l'approximation de l'integrale de Riemann par les sommes

# Sortie : une liste a 2 elements, chacun comprenant une matrice dont la premiere colonne contient les nombres de spikes par essai, puis les positions
# ------------------------------------------------------------------------------------- #

source('simulationPPnonhom.R')

simulPPI <- function(Temps,Sauts,taux,nessai)
{
	Q=list() 							# liste contenant les positions des tops par essais
	ntops=c()							# vecteur contenant le nb de tops de chaque essai
	N=2*nessai
	for(i in 1:N)
	{	
		PP=PPI_thin(Temps,Sauts,taux)
		ntops=c(ntops,length(PP))
		Q=c(Q,list(PP))
	}
	s=max(ntops)
	M=matrix(data=0,nrow=N,ncol=s+1)
	M[,1]=ntops			# la premiere colonne de M contient le nombre de tops
	for (i in 1:N)		# puis on remplit la matrice ligne par ligne
		{	r=ntops[i]	
		if (r!=0)
		{	M[i,2:(r+1)]=Q[[i]]
		}
	}
	simu=list(M[1:nessai,],M[(nessai+1):(2*nessai),])
	return(simu)
}

