intensity<-function(time, past, pasttype, spont, interac)

	# Calcule l'intensite du processus de hawkes multivarie au temps time pour tous les types avec les points du
	# passe dans past et leur type dans pasttype, Il ya Nbtype types.
	# spont donne les valeurs respectives des apparitions spontanees, taille = Nbtype
	# interac est une matrice de cellules de taille Nbtype * Nbtype ,
	# ligne = chez qui on contamine, colonne = celui qui contamine
	# chaque cellule contient une matrice a deux lignes donnant partition et valeur de
	# la partition. Il faut que toutes les partitions commencent par 0 et
	# soient a valeurs positives. Le chiffre sous 0 correspond a la valeur en 0+
	#  de la fonction d'interaction, la valeur en 0 de la fonction doit etre
	#  comprise comme 0. La derniere valeur des valeurs sur la partition doit etre 0.
	# Attention time >= past

{
Nbtype		= length(spont)										# on recupere le nob de type
interacIntegral	= matrix(data=0,nrow=Nbtype,ncol=Nbtype)  					# ca va etre la matrice des \int^(time^-) h_col^lin(time-u) dN^col_u
for (col in 1:Nbtype)
	{
	pastcol	= past[which(pasttype==col)]
	diff	= time-pastcol
	for (lin in 1:Nbtype)
		{
		hcollin			= interac[lin,col][[1]]						# on recupere la valeur de la fonction d'interca sous forme part, valpart
        	bh				= sort(c(diff, hcollin[1,]),index.return=TRUE)        #on range
        	Nbpastcol			= length(pastcol)
        	pastcolPos			= 1*(bh$ix<=Nbpastcol)
		    partPos			    = 1*(bh$ix>Nbpastcol)                                 # on retrouve qui est ou
        	value				= c(0,hcollin[2,])						# les valeurs possibles, je rajoute 0 pour le cas ou il y ait des diff=0
        	index				= cumsum(partPos)+1						# donne les indices a prendre meme pour la partition
        	indexPast			= index[which(index*pastcolPos!=0)]				# donne les indices a prendre que pour pastcol
		interacIntegral[lin,col]= sum(value[indexPast])						# je somme sur tous les points
		}
	}
intensity		= pmax(spont+rowSums(interacIntegral),matrix(data=0,nrow=Nbtype,ncol=1))# on prend la partie positive
}

#######################
######################


future<-function(time, past, pasttype, spont, interac,interacMaj,M)

	# cette fonction va etre appelee iterativement pour proposer un nouveau
	# temps qui sera ou non eventuellement un nouveau point (et alors il sera
	# mis dans le passe en derniere position), 
	# memes arguments que la fonction intensity sinon + les majorants necessaires ...
	# M[col] = sum_lin h^maj_col->lin

{	
Nbtype	= length(spont)									# on recupere le nombre de types
 lpast=length(past)
	# Il faut d'abord avoir une "intensite" majorante de la somme sur les types des intensites, 
	# decroissante en l'absence de nouvelles apparitions donnees par spontMaj inetracMaj,
	# cf fonction HAwkes -> calcule en dehors de future passe en argument
 	# il faut ensuite savoir si c'est un vrai point ou pas, si c'en est un
	# c'est focement le dernier car time>=past et past range par ordre croissant

if (lpast==0)
	{	
	LambdaMaj = sum(intensity(time,past,pasttype,spont,interacMaj))		# si pas de passe, time n'est pas un point et comme majorant decroissant ca majore 
													# l'intensite dans le futur
	}
else
	{
	if (time==past[lpast])
     		{
     			
     		### Majorant de la somme des trucs en plus du au saut qu'on rajoute a l'intensite	
     		LambdaMaj = sum(intensity(time,past,pasttype,spont,interacMaj))+M[pasttype[lpast]];
		}
	else
	# c'est un  majorant de l'intensite cumulee sur tous les types pour le
	# futur apres time qui est un point. Il n y a qu'un seul type pour
	# l'intensite majorante
		{
  		LambdaMaj = sum(intensity(time,past,pasttype,spont,interacMaj)); 	# idem qu'avant pour pas de passe
  		} 
	}  

U	= runif(1, min=0, max=1)
E	= -log(U)/LambdaMaj                                                   	# cree une exponentielle de param LambdaMaj : c'est le prochain candidat point
 
newtime	= time+E                                         
 
													# il reste a savoir si c'est un vrai point et si oui quel est son type.
 
lambda	= intensity(newtime,past,pasttype,spont,interac)
 
													# on a tout fait pour sum(lambda) sur tous les types <=LambdaMaj
 
somme		= cumsum(lambda)

V		= runif(1, min=0, max=1)
 
if (V<somme[Nbtype]/LambdaMaj) 
	{												# alors c'est un point
	newtype		= min(which(somme/LambdaMaj-V>0))    
      newpast		= c(past, newtime)
      newpasttype		= c(pasttype, newtype) 	        
	}
else                                                                                                                                             
      {
      newpast		= past
      newpasttype		= pasttype
      }

future	= list(newtime, newpast, newpasttype)
}

############
############

Hawkes<-function(DF,past,pasttype,spont,interac,interacMaj,M)

	# renvoie les points simules entre DF=[debut fin] avec le passe qu'il faut
	# avant debut et ce jusuq'a fin selon parametres donnes

{
points			= c()
pointstype		= c()
debut			= DF[1]
fin			= DF[2]

	# on creee l'intensite majorante de la somme sur les types des intensites, decroissante en
	# l'absence de nouvelles apparitions donnees par spontMaj inetracMaj
 
	# on cree ici interacMaj et spontMaj correspondante qui n'ont qu'un seul type !

Nbtype			= length(spont)								# on recupere le nombre de types
 
R			= 0
for (lin in 1:Nbtype)
	{
    	for ( col in 1:Nbtype)
      		{  
        	hcollin		= interacMaj[lin,col][[1]]
        	R		= max(c(R,hcollin[1,]))
   		}
	}

	# on censure le passe qui ne sert a rien
 
vrai			= which(debut-past<=R)
past			= past[vrai]
pasttype		= pasttype[vrai]
 
F			= future(debut, past, pasttype, spont, interac, interacMaj,M);		# donne le point d'apres
newtime		= F[[1]]
newpast		= F[[2]]
newpasttype	= F[[3]]
	
while (newtime<=fin)										# tant qu'on deborde pas du temps de sim
	{ 
	if ((length(newpast)>0)&(newtime==newpast[length(newpast)]))				# si c'est un vrai point on le garde
		{
		points		= c(points,newtime)
		pointstype	= c(pointstype,newpasttype[length(newpast)])
		}
	else
		{
		}
    
	time		= newtime
	vrai		= which(time-newpast<=R)
	past		= newpast[vrai]                            	                  	# j'enleve le passe qui ne sert plus
	pasttype	= newpasttype[vrai]
 
    
	F		= future(time, past, pasttype, spont, interac, interacMaj,M);
	newtime	= F[[1]]
	newpast	= F[[2]]
	newpasttype	= F[[3]]
	}
	
Hawkes		= list(points, pointstype)
}

#########

########

Hawkesmulti<-function(Nessai,delai,CaseComp,spontComp,interacComp)

	# renvoi les donnees sous la forme Data avec Nessai, le nombre d'essai desire 
	# CaseComp vecteur de tailles Nessai compose de cellule, chaque cellule
	#contient matrice a 3 lignes
	# ligne 1 de CaseComp : debut case
	# ligne 2 de CaseComp : fin case
	# ligne 3 de CaseComp : le comportement de la case 
	# spontComp : matrice Nneur lignes, Ncomp (ie le nombre de comportements) colonnes
	# interacComp : matrice de cellules a 3 dim pour 1er indice lin, 2eme indice
	# col, 3 eme indice comp, donne la fonction qui contamine chez neurone lin,
	# grace au neurone col pour le comportement comp
	# Au debut de chaque essai, on va rester le temps delai dans le premier etat
	# (repos) et relative independance
	# On suppose que les Cases  de CaseComp sont rangees dans l'ordre croissant ,essai apres
	# essai.
{
Nneur		= nrow(spontComp)
Ncomp		= ncol(spontComp)


InteracMaj	= array(data=c(list()),c(Nneur,Nneur,Ncomp)) # future mattrice de majorant des interactions sous forme partition /valeur de la partition
M=matrix(data=0,ncol=Ncomp,nrow=Nneur) # la somme des majorants pres de 0 pour la valeur intensite si saut comportemtn en colonne, neurone en ligne

for (k in 1:Ncomp)	
{
	for (j in 1:Nneur)
		{
		for (i in 1:Nneur)
			{
			a			= interacComp[i,j,k][[1]]
			l			= ncol(a)
			a1			= a[1,] #partition
			a2			= a[2,] # valeur de la partition
			
			#### les tests pour que tout se passe bien
			
			if(length(a1)<2)
			{
				print(paste('Error, dimension of the partition',i,'<-',j,'too small',sep=' '))
				break
			}
			if(a1[1]!=0)
			{
				print(paste('Error, first point of the partition',i,'<-',j,'not 0',sep=' '))
				break
			}
			if(prod(a1==sort(a1))==0)
			{
				print(paste('Error, partition',i,'<-',j,'not in increasing order',sep=' '))
				break
			}
			if(a2[l]!=0)
			{
				print(paste('Error, last value for the partition',i,'<-',j,'not zero',sep=' '))
				break
			}

			# intitialisation
			
			h1			= c(0) # future partition maj
			h2			= c() # future valeur de partition maj
			maxx			= a[1,l]
			mi			= a[1,1]
			
			while ( mi<=maxx) # on rentre forcement dans les boucles vu les tests avec un a[1,1]=0 et maxx> mi, mi sera updated
				{
				la1	= length(a1) # si on est la c'est forcement plus grand que 2
				
				maxi	= max(a2) # le plus gros coeff
				tmaxi	= max(which(a2==maxi)) #la pos du plus gros coeff 
				if (tmaxi<la1)
					{
						h1	= c(h1,a1[tmaxi+1]) # la coupure qui correspond au plus gros coeff est en tmaxi+1 ... mis ici dans future partition
						h2	= c(h2,maxi) # la valeur correspondante
						a1	= c(0,a1[-c(1:(tmaxi+1))]) # on prend que ce qui reste dans a1
						a2	= a2[-c(1:tmaxi)] # et pareil dans a2
					}
				else
					{
						h1	= c(h1,a1[tmaxi])
						h2	= c(h2,maxi)
						a1	= c(0) # si j'ai enleve toute les valeurs critiques il ne reste que 0
					}
				if (length(a1)==1)
					{
						mi	= maxx+1 # ben du coup on sortira de la boucle
					}
				else
					{
						mi	= a1[1] #bon en principe ca reste 0
					}
				
				}
			h2			= c(h2,0) # on complete pour que ca est meme taille
			A			= matrix(data=0,nrow=2,ncol=length(h2))
			A[1,]			= h1
			A[2,]			= h2
			InteracMaj[i,j,k][[1]]	=A
			M[j,k]=M[j,k]+InteracMaj[i,j,k][[1]]	[2,1] ## nb: premiere valeur de interacMaj toujours positive ou nulle car 0 en fin de valeur partition
			}
		}	
	}							

Data		= array(data=c(list()),c(Nneur,Nessai))   # initialisation sous forme de liste

nbpt=matrix(data=0,nrow=Nneur,ncol=Nessai) # futur nombre de points max

for (essai in 1:Nessai)
	{
	CaseEssai	= CaseComp[[essai]]  
	ncase	 	= ncol(CaseEssai)
    
    
	# on commence par faire evoluer sur un temps delai
 
	comp		= CaseEssai[3,1]
 	DF		= c(-delai,0)
	H 		= Hawkes(DF,c(),c(),spontComp[,comp],interacComp[,,comp],InteracMaj[,,comp],M[,comp])
 	past		= H[[1]]
	pasttype	= H[[2]]
 	points		= c()
    pointstype	=c()
    
	for (nc in 1:ncase)
		{
        	DF		= c(0,0)
        	DF[1]		= CaseEssai[1,nc]
        	DF[2]		= CaseEssai[2,nc]
        	comp		= CaseEssai[3,nc]
      	    H		= Hawkes(DF,past,pasttype,spontComp[,comp],interacComp[,,comp],InteracMaj[,,comp],M[,comp])
        	pcase		= H[[1]]
        	tcase		= H[[2]]
        	past		= c(past, pcase)
        	pasttype	= c(pasttype, tcase)
        	points	= c(points, pcase)
        	pointstype 	= c(pointstype, tcase)					# j'ai tous mes points pour l'essai mes pas mis sous forme Data
		}
												
    for (nneur in 1:Nneur)
        {
        	pointneur		= points[which(pointstype==nneur)]		# on recupere les points du neurone
        	nbpt[nneur,essai]=length(pointneur)
        	Data[nneur,essai][[1]]=pointneur	 						# on met dans une liste
        	
        }
   }

#### on remet sous le bon format
   
nmax= max(nbpt)
DataNeur=matrix(data=0,nrow=Nessai*Nneur,ncol=nmax+1)

for (essai in 1:Nessai)
	{
	for (nneur in 1:Nneur)
        {
        	indice		= Nessai*(nneur-1)+essai                  # indice correspondant dans Data
        	DataNeur[indice,1] = nbpt[nneur,essai]                                     # on met le nombre de vrai points
        	if (DataNeur[indice,1]>0)
			    {
        		DataNeur[indice,2:(DataNeur[indice,1]+1)] = Data[nneur,essai][[1]]
			    }   
        }
   }     
Hawkesmulti	= DataNeur							
} 

