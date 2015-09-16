bnc<-function(Nnc,A)						
	# calcul du vecteur bn pour la case c
	# Nnc : temps des spikes du neurone n pour l'essai e et
	# entre deb et fin (de la case c)
	# del : valeur de delta
	# A : tableau de l'ensemble des N_i^j associes a l'essai e
{
d	= dim(A)
k	= d[3]
Nneur	= d[4]
V	= matrix(data=0,ncol=1,nrow=(1+k*Nneur))
r	= 2
for ( j in 1:Nneur)
	{
	for ( i in 1:k)
		{
		B	= A[,,i,j]
		V[r,1]= intijnc(Nnc,B)
		r	= r+1
		}
	}
V[1,1]= length(Nnc)
bnc	= V
}

