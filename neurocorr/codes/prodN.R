prodN<-function(phi,Nessai,Nneur,k)
{
AA = array(data=c(list()),c(k*Nneur,k*Nneur,Nessai))								# structure de stockage
for (nessai in 1:Nessai)
	{
	for (nneur in 1:Nneur)
		{
		for (i in 1:k)
			{
			#print(c(nessai,nneur,i))	
			x = k*(nneur-1)+i											# ligne associee au neurone nneur et a l'element numero i de la partition
			if (x==k*Nneur)											# attention, par essai, on construit une matrice et si dernier element diagoanal,
																# qu'un seul calcul a effectuer, celui du produit
																# du N^nneur_[t-i*del,t-(i-1)*del] par lui-meme
				{
				Do			= phi[,,i,nneur,nessai]
				nc			= ncol(Do)
				b			= matrix(data=0,nrow=2,ncol=nc)
				b[1,]			= Do[1,]
				b[2,]			= Do[2,]^2
				AA[x,x,nessai]	= list(b)
				}	
			else													# pour des raisons de symetrie, on commence par remplir la matrice par les elements de la ligne x apres l'element diagonal
				{
				for (indice in (x+1):(k*Nneur))
					{
					l		= indice%%k								# attention on a besoin de l et m tels que indice=k*(m-1)+l (m numero du neurone et l numero de l'element de la partition)
																# donc on definit l comme etant indice mod k mais alors l entre 0 et k-1 alors que nous l entre 1 et k
																# il faut faire correspondre le 0 obtenu ici a la valeur k et adapter la valeur de m
					#print(indice)											
					if (l==0)
						{
						l	= k
						m	= indice%/%k
						}
					else
						{
						m	= indice%/%k+1
						}
					A				= phi[ , , i,nneur,nessai]			# recuperation des donnees associees
					B				= phi[ , , l,m,nessai]
					a				= produit(A,B)					# calcul du produit desire
					a1				= matrix(data=0,nrow=2,ncol=ncol(a))
					a1[1,]			= a[1,]
					a1[2,]			= a[4,]
					AA[x,indice,nessai]	= list(a1)						# stockage
					AA[indice,x,nessai]	= list(a1)						# remplissage du symetrique
					}
				Do			= phi[,,i,nneur,nessai]						# calcul de l'element diagonal de la ligne x
				nc			= ncol(Do)
				b			= matrix(data=0,nrow=2,ncol=nc)
				b[1,]			= Do[1,]
				b[2,]			= Do[2,]^2
				AA[x,x,nessai]	= list(b)
				}
			}
		}
	}
prodN	= AA
}

