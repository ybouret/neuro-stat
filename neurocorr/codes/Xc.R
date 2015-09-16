Xc<-function(Nije,prode,DF,k,Nneur)								
	# Nije : objet issu de la fonction Nij pour l'essai associe a la case c
	# prode : objet issu de la fonction prodN pour l'essai associe a la case c
{
deb		= DF[1]
fin		= DF[2]
A		= matrix(data=0,ncol=(k*Nneur+1),nrow=(k*Nneur+1))			# initialisation
A[1,1]	= fin-deb										# premiere case : longueur de la case
for (y in 2:(k*Nneur+1))									# remplissage de la premiere ligne et premiere colonne
	{
	i		= (y-1)%%k									# recherche de l'element de la partition associe avec les memes precautions
	if (i==0)
		{
		i=k
		nneur		= (y-1)%/%k								# recherche du neurone associe avec les memes precautions
		}
	else
		{
		nneur		= (y-1)%/%k+1
		}
	X		= Nije[,,i,nneur]								# extraction des donnees N_i^Nneur
	nX		= length(which(X[1,]!=0))						# attention de retirer les 0 de la fin
	X		= X[,1:(nX+1)]
	a		= fintegrale(X,DF)							# calcul de l'integrale des N_i^Nneur
	A[1,y]	= a										# stockage sur premiere ligne
	A[y,1]	= a										# et symetrie
	for (x in y:(k*Nneur+1))								# boucle pour remplir le reste
		{
		X		= prode[(y-1),(x-1)][[1]]					# attention, chaque element de prode est une liste a un element d'ou le [[1]]
		nX		= length(which(X[1,]!=0))					# attention de retirer les 0 de la fin
		X		= X[,1:(nX+1)]
		a		= fintegrale(X,DF)						# calcul de l'integrale associee au produit
		A[y,x]	= a									# stockage
		A[x,y]	= a									# symetrie
		}
	}
Xc		= A
}

