Nij<-function(Data,Nessai,Nneur,k,del)   				
	# Data, matrice avec la premiere colonne qui contient le nombre effectif de donnees par enregistrement, 
	# et ensuite le temps des spikes (le nombre
	# de colonnes est le nombre maximal de spike sur l'ensemble des enregistrements
	# l'enregistrement associe au neurone m et a la repetition i est sur la ligne Nessai*(m-1)+i
	# Nessai, le nombre de repetitions
	# Nneur, le nombre de neurones enregistres simultanement
	# k le nombre d'elements de la partition
	# de longueur de chacun des elements de la partition
{
nmax	= ncol(Data)-1
phi	= array(data=0,c(2,(2*nmax+1),k,Nneur,Nessai))							# structure de stockage des N^j_[t-i*del,t-(i-1)*del] pour tout neurone j, 
															# tout element i de la partition et tout essai 
															# attention beaucoup de 0 a la fin des lignes pour avoir une structure commune
for (nessai in 1:Nessai)
	{
	for (nneur in  1:Nneur)
		{
		#	print(list(nessai,nneur))
		n						= Data[Nessai*(nneur-1)+nessai,1]		# recuperation du nombre effectif de spikes sur l'enregistrement considere
		if(n>0)
			{
			U						= Data[Nessai*(nneur-1)+nessai,2:(n+1)]	# recuperation des differents temps de survenue des spikes
			W1						= matrix(data=0,nrow=2,ncol=(2*n+1))	# initialisation
			W1[1,2:(2*n+1)]				= rep(c(0,del),times=n)+rep(U,each=2)	# stockage des temps de spikes + (i-1)*del et des temps de spikes + i*del (en alternance)
			W1[2,2:(2*n+1)]				= rep(c(1,-1),times=n)				# stockage des 1 et -1 associes (1 pour temps de spikes + (i-1)*del)
			W2						= W1[,sort.list(W1[1,])]			# on reordonnes les temps stockes precedemment par ordre croissant
			W3						= matrix(data=0,nrow=2,ncol=(2*n+1))	# matrice d'initialisation
			W3[1,]					= W2[1,]						# conservation des temps de "saut"
			W3[2,]					= cumsum(W2[2,])					# valeurs des N^nneur_[t-del,t] pour l'essai numero nessai
			phi[,1:(2*n+1),1,nneur,nessai]	= diag(c(1,1/sqrt(del)))%*%W3			# ne pas oublier la normalisation
			for (i in 2:k)											# calcul des N^nneur_[t-i*del,t-(i-1)*del] pour l'essai numero nessai pour i allant de 2 a k 
															# par le lien de decalage
				{
				phi[2,,i,nneur,nessai]			= phi[2,,1,nneur,nessai]
				phi[1,2:(2*n+1),i,nneur,nessai]	= phi[1,2:(2*n+1),1,nneur,nessai]+del*(i-1)
				}
			}
		else{
			for (i in 1:k)											# calcul des N^nneur_[t-i*del,t-(i-1)*del] pour l'essai numero nessai pour i allant de 2 a k 
															# par le lien de decalage
				{	
				phi[1,2,i,nneur,nessai]=del	
				}
			}
		}	
	}
Nij	= phi
}

