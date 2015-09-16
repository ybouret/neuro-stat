Xtc<-function(Nijt,prodt,k,Nneur,Case)				
	# Nijt : objet issu de Nij
	# prodt : objet issu de prodN
	# Case : matrice contenant les informations sur les cases ( 3lignes avec
	# la 1ere le numero de l essai associe
	# la 2nd le temps de debut des cases
	# la 3eme le temps de fin des cases
{
Ncase	= ncol(Case)							# nbre de cases
AA	= array(data=c(list()),c(1,Ncase))				# structure de stockage
for (j in 1:Ncase)
	{
	e		= Case[1,j]						# extraction du numero de l essai
	Nije		= Nijt[,,,,e]					# extraction des N_i^j associes a l'essai e
	prode		= prodt[,,e]					# extraction des produits associes a l'essai e
	DF		= Case[c(2,3),j]					# extraction du vecteur debut et fin de la case
	a		= Xc(Nije,prode,DF,k,Nneur)			# calcul de la matrice Xc associee
	AA[1,j]	= list(a)						#stockage
	}
Xtc	= AA
}

