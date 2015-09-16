vb<-function(Data,Case,phic)					
	# calcul de tous les vecteurs bnc
	# phic : objet issu de la fonction Nij
{
d	= dim(phic)
Nneur	= d[4]
k	= d[3]
Nessai=d[5]
Nc	= ncol(Case)
U	= array(data=c(list()),c(Nneur,Nc))
for ( n in 1:Nneur)							#boucle sur le numéro du neurone
	{
	for (nc in 1:Nc)								#boucle sur les cases
		{
		e	= Case[1,nc]								#numéro de l'essai associe à la case nc
		DF	= Case[2:3,nc]								#vecteur de debut et fin de la case nc
		Nn	= Data[Nessai*(n-1)+e,]
		Nn	= Nn[-1]									#les temps des spikes du neurone n a l'essai e
		deb	= DF[1]
		fin	= DF[2]
		Nnc1	= ( Nn > deb )*( Nn <= fin )
		a	= which(Nnc1==1)
		if (length(a)!=0)
			{
			Nnc		= Nn[a]
			A		= phic[,,,,e]
			b		= bnc(Nnc,A)
			U[n,nc]	= list(b)
			}
		else
			{
			U[n,nc]	= list(matrix(data=0,ncol=1,nrow=(1+k*Nneur)))
			}
		}
	}
vb	= U
}
