produit<-function(A,B)								#calcul du produit entre deux donnees stockees dans un objet issu de la fonction Nij
{
nA			= length(which(A[1,]!=0))				#recuperation du nombre effectif de donnees
nB			= length(which(B[1,]!=0))
A1			= as.matrix(A[,2:(nA+1)])						#recuperation de la partition de temps associe a l'objet A
B1			= as.matrix(B[,2:(nB+1)])
C			= matrix(data=0,nrow=4,ncol=(nA+nB+1))		#initialisation
ts			= sort(c(A1[1,],B1[1,]),index.return=TRUE)	#creation de la partition fine de temps associee aux objets A et B, ts$x est les temps reordonnes par ordre croissant
											#ts$ix est la position initiale de ces nouveaux temps dans le vecteur initial
C[1,2:(nA+nB+1)]	= ts$x							#stockage des temps de la partition fine en conservant un 0 en premier temps
posA			= 1*(ts$ix<=nA)						#vecteur binaire avec 1 si le temps associe etait issu de l'objet A et 0 sinon
posB			= 1*(ts$ix>nA)
posAf			= cumsum(posA)+1						#pour pouvoir associe les valeurs aux temps
posBf			= cumsum(posB)+1
C[2,2:(nA+nB+1)]	= A[2,posAf]						#extraction des valeurs
C[3,2:(nA+nB+1)]	= B[2,posBf]
C[4,]			= C[2,]*C[3,]						#calcul du produit
produit		= C
}

