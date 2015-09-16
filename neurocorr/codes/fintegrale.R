fintegrale<-function(A,DF)								
	# A : matrice deux lignes et DF vecteur a deux elements (bornes de l'integrale)
	# premiere colonne de A que des 0
	# A objet dont on veut calculer l'integrale
{
deb = DF[1]											# borne inferieure de l'integrale
if (deb<0)
	{
	fintegrale = c("erreur le debut doit etre positif ou nul")
	}
else
	{
	fin		= DF[2]								# borne superieure de l'integrale
	part		= A[1,]									
	partf		= (part>=deb)*(part<fin)					# localisation des temps de A dans [deb,fin[
	indice	= which(partf==1)							# position des temps precedents
	if (length(indice)>=1)
		{
		b		= c(part[indice],fin)					# ajout du temps fin pour le calcul integral
		v		= A[2,indice]						# recuperation des valeurs de la fonction dont on veut l'integrale
		a		= indice[1]
		if (a!=1)									# si deb ne vaut pas 0, on ajoute le temps de debut quitte a ajouter un intervalle
												# de longueur nulle
			{
			b	= c(deb,b)
			v	= c(A[2,(a-1)],v)
			}
		dif		= diff(b)
		fintegrale	= sum(dif*v)
		}
	else
		{
		indiceb	= which(part<deb)
		fintegrale	= (fin-deb)*A[2,indiceb[length(indiceb)]]
		}
	}
}

