# Sommaire :
# 	- fonction cloche
#	- fonction intensite a plusieurs plateaux comprenant chacuns une bosse
#	- approximation de l'aire de la fonction intensite
#	- simulations d'un processus de Poisson par deux methodes : le thining ou le rejet

# ------------------------------------------------------------------------------------- #
# Simulation d'un processus de Poisson non homogene
# ------------------------------------------------------------------------------------- #

# Remarque : Conditionnellement au nb de pts du processus, les tops suivent une loi de densite intensite sur [a,b]

# ------------------------------------------------------------------------------------- #
# construction d'une fonction cloche :
# ------------------------------------------------------------------------------------- #

cloche = function(t,centre,r,hauteur)
{	
	z=(t-centre)/r
	c=hauteur*exp(-4 * z^2 * ( 1 - z^2 )^(-1) )
	return(c)
}

# ------------------------------------------------------------------------------------- #
# construction de la fonction intensite (a 5 plateaux) : 
# ------------------------------------------------------------------------------------- #
	
# le vecteur T contient les differentes abscisses pour les plateux ( T=(a,t1,t2,t3,t4,b) )
# le vecteur S contient les differentes hauteurs des plateux ( S=(0,s1,s2,s3,0) )
# taux regle la taille des bosses sur chaque plateau (doit etre compris entre 0 et 1)
# la densite a son expression pour des t dans [0,1], pour le ramener a un intervalle [a,b], on applique lambda a (t-a)/(b-a), 

intensite <- function(t,T,S,taux)
{
	# si t n'est pas dans [a,b[, alors c'est 0
	if( (t<min(T)) + (t>=max(T)) ==1 )
	l=0
	else
	{
		# on regarde dans quelle fenetre est t :
		i=max(which(t>=T))
		
		if(T[i]-t==0) # ie t est dans T (prive de b)
		{
			return(S[i])
		}
		else
		{
			c=(T[i+1]+T[i])/2 # centre de la fenetre
			r=(T[i+1]-T[i])/2 # "rayon" de la fenetre
			if(i==(length(T)-1))
			{
				haut=taux*(S[2]-S[1])
			}
			else
			{
				haut=taux*abs(S[i+1]-S[i])
			}
			l=S[i]+cloche(t,c,r,haut)
		}	
		return(l)
	}
}

# ------------------------------------------------------------------------------------- #
# Graphe de l'intensite
# ------------------------------------------------------------------------------------- #

graphe_intensite <- function(T,S,taux)
{
	a=T[1]
	b=T[length(T)]
	pas=(b-a)/1000

	x=seq(a-50*pas,b+50*pas,pas)
	
	L=c()
	for(j in 1:length(x))
	{	
		l=intensite(x[j],T,S,taux)
		L=c(L,l)
	}
	
	plot(x,L,type='b',main='Inhomogeneous Poisson Process Intensity Graph',xlab='time',ylab='intensity')
}

# ------------------------------------------------------------------------------------- #
# approximation de l'aire de t->intensite(t,T,S,taux) sur [alpha,beta] inclus dans [a,b]
# par les sommes de Riemann (pour nR pas) :
# ------------------------------------------------------------------------------------- #

# aire <- function(alpha,beta,nR,T,S,taux)
# {
	# subd=seq(alpha,beta,(beta-alpha)/nR)
	# L=c()
	# for(k in 1:nR)
	# L=c(L,intensite(subd[k],T,S,taux))
	
	# A=((beta-alpha)/nR)*sum(L)
	# return(A)
# }

# ------------------------------------------------------------------------------------- #
# Simulation des processus d'un processus de Poisson d'intensite intensite :
# par la methode du thining. 
# ------------------------------------------------------------------------------------- #

PPI_thin <- function(T,S,taux)
{	
	# on aimerait connaitre un majorant de l'intensite :
	M=S[length(S)-1]*(1+taux)
	
	N=c()
	
	top=T[1]
	
	while(top < T[length(T)])
	{
		saut=rexp(1,M)
		U=runif(1,min=0,max=M)
		top = top+saut
		if(U <= intensite(top,T,S,taux))
		{
			N=c(N,top)
		}
	}
	
	return(N)
}
	
	
# ------------------------------------------------------------------------------------- #
# Simulation des processus d'un processus de Poisson d'intensite intensite :
# par la méthode du rejet. 
# ------------------------------------------------------------------------------------- #

PPI_rejet <- function(T,S,taux)
{
	a=T[1]
	b=T[length(T)]
	
	# on aimerait connaitre un majorant de l'intensite :
	M=S[length(S)-1]*(1+taux)

	Nb=rpois(1,M*(b-a))

	U=runif(Nb,min=0,max=(M))
	V=runif(Nb,min=a,max=b)

	Y=c()
	for(i in 1:Nb)
	{
		Y[i]=intensite(V[i],T,S,taux)
	}

	W=which(U<=Y)
	N=V[W]

	return(sort(N))
}



