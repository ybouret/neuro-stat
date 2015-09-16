intijnc<-function(Nnc,A)			
	#A : matrice 2 lignes avec les N_i^j de l'essai e
	#e etant l'essai associe a la case c					
	#Nnc : temps des spikes du neurone n pour l'essai e et					
	# entre deb et fin (de la case c)
{
l		= length(which(A[1,]!=0))
A		= A[,1:(l+1)]			#retrait des 0 de la fin
t		= c(Nnc,A[1,2:(l+1)])
ts		= sort(t,index.return=TRUE)
tsix		= ts$ix
ll		= length(Nnc)
pNn		= 1*(tsix<=ll)
pA		= 1*(tsix>ll)
cpA		= cumsum(pA)+1
b		= cpA*pNn
s		= sum(A[2,b])
intijnc	= s
}

