############## THINNING method for Hawkes processes with piecewise constant interaction functions
############### without decreasing upperbounds on  the intensity

sumpcw<-function(A,B)
{
	##### this function takes two piecewise constant "functions" A and B coded as partition and value of the partition with first value of the partition 0 for both
	ncolA=ncol(A)
	ncolB=ncol(B)
	
	if(ncolA==1) ### if just one value almost nothing to do
	{
		B[2,]=B[2,]+A[2,1]
		return(B)
	}else{
		if(ncolB==1)
		{
			A[2,]=A[2,]+B[2,1]
			return(A)
		}else{														#that's the most difficult part
			nA			= ncol(A)-1									# number of non zero values in the partition in A
			nB			= ncol(B)-1									# number of non zero values in the partition in B
			A1			= as.matrix(A[,2:(nA+1)])					# non zero	values in A			
			B1			= as.matrix(B[,2:(nB+1)])					# non zero values in B
			C			= matrix(data=0,nrow=4,ncol=(nA+nB+1))		#initialization
			ts			= sort(c(A1[1,],B1[1,]),index.return=TRUE)	# the new values of the partition A union B without 0
	
			C[1,2:(nA+nB+1)]	= ts$x									# first line of C = the values of the partition with the missing 0
			posA			= 1*(ts$ix<=nA)						 		# find the positions that were originally in the partition A and mark them by 1	
			posB			= 1*(ts$ix>nA)								# idem for B
			posAf			= cumsum(posA)+1							# find the indices corresponding to the values that should be looked for in A
			posBf			= cumsum(posB)+1							# idem for B
			C[2,2:(nA+nB+1)]	= A[2,posAf]								# Extract the corresponding values
			C[3,2:(nA+nB+1)]	= B[2,posBf]
			C[2,1]=A[2,1]											# first value in 0 was missing
			C[3,1]=B[2,1]
			C[4,]			= C[2,]+C[3,]							# do the sum
			
			doublons= which(diff(C[1,])==0)
			if(length(doublons)!=0){C=C[,-doublons]}
			return(C[c(1,4),])
		}
	}
}

cutting <- function(elapsed,A)
{
	#### this function cancels and shifts the piecewise constant function A starting in 0 to elapsed which becomes the new 0 . 
	#### elapsed should be >0
	
	index=which(elapsed<A[1,]) 												### find the upperbound of the interval where elapsed should be
	if(length(index)==0)
	{
		A=matrix(c(0,A[2,ncol(A)]),nrow=2) 									# if it's larger than the last cut then it remains only the last value... should be the spontaneous part
	}else{
		A=rbind(c(0,(A[1,index]-elapsed)),c(A[2,(min(index)-1)],A[2,index]))			# cut the partition put the 0 at the right place and shift the values
	}
	return(A)
}


update <- function(elapsed,type,psi,interac)
{
	##### this function updates the intensity system : it returns new psi (ie the linear predictable process), new intensity (ie the positive part of psi) and new intensitySum (the sum of the intensities over all types)
	##### elapsed is the new elapsed time at which the intensity system has to be updated. type is its type
	##### psi is a list of piecewise constant functions as before. Each index  of the list corresponds to a type. Since it is only 
	##### useful for the future, always the same convention: start with 0 for the partition which corresponds to the value at real time.
	##### same thing for intensity and intensitySum.
	##### interac is an array Ntype*Ntype of interactions functions, all piecewise constants starting in 0
		
     ##### update of psi
	
	Ntype=length(psi)
	newpsi=lapply(1:Ntype,FUN=function(i)
	{
		a=cutting(elapsed,psi[[i]])          # same thing for each intensity
		a=sumpcw(a,interac[i,type][[1]])
	    return(a)
	})	
	
	#### update of intensity
	
	newint=lapply(1:Ntype,FUN=function(i)
	{
		a=newpsi[[i]]          # same thing for each intensity
		a[2,]=a[2,]*(a[2,]>0)
	    return(a)
	})	
	
	##### update of intensitySum
	
	newintSum=Reduce(sumpcw,newint)
	
	
	return(list(psi=newpsi,intensity=newint,intensitySum=newintSum))
}

future <- function(intensity,intensitySum)
{
	#### this function gives the future time and its future type given the current intensity and intensitySum
	
	U	= runif(1, min=0, max=1) # the random variable which will give the future time
	
	nc	= ncol(intensitySum)    # for the moment do as if the current location attime is 0
	if(nc==1)
	{
		elapsed = -log(U)/intensitySum[2,1]  # if just one column it's a homogeneous poisson process and  an exponential variable is sufficient
	}else{
		
		cs 	 = cumsum(diff(intensitySum[1,])*intensitySum[2,1:(nc-1)])  # the different times where the regimes change in the -ln(U) world
		lcs  =length(cs)
		if(cs[lcs]<= (-log(U)))
		{
			elapsed = (-log(U)-cs[lcs])/intensitySum[2,nc]+intensitySum[1,nc] ### we need to look in the last unbounded interval and that's the formula
		}else{
			index= which(cs<=(-log(U)))  ### the ones smaller than (-log(U))
			if(length(index)==0)
			{
				elapsed= -log(U)/intensitySum[2,1]  ## same formula as if just one column
			}else{
			index=max(index)	
			elapsed = (-log(U)-cs[index])/intensitySum[2,(index+1)]+intensitySum[1,(index+1)] ### same formula as before with the right choices
			}
		}	
			
	}
	
	##### the type corresponding to time
	
	V		= runif(1, min=0, max=1) #### the random variable for the type
	
	Ntype=length(intensity)
	int=sapply(1:Ntype,FUN=function(i){
		ind=max(which(intensity[[i]][1,]<elapsed))  ### index to look for in the partition
		return(intensity[[i]][2,ind])
		})
		
	# vector(length=Ntype)  ##### initialisation
	
	# for(i in 1:Ntype)
	# {
		# ind=max(which(intensity[[i]][1,]<elapsed))  ### index to look for in the partition
		# int[i]=intensity[[i]][2,ind]   ### its value
	# }
	
	sumint		= cumsum(int)  ### the cuts for V
											
	type		= min(which(sumint>V*sumint[Ntype]))    
       
    return(list(elapsed=elapsed,type=type))       ## return the elapsed time 
}

Hawkes <- function(DF,psi,intensity,intensitySum,interac)
{
	#### function which initiated with an intensity and intensitySum valid for time =DF[1] simulates the Hawkes  on DF[1], DF[2] and returns
	#### vector of points, vector of types, intensity and intensitySum valid at time DF[2]
	
	#### initialization
	
	points			= c()
	pointstype		= c()
	
	
	Ntype			= length(intensity)
	
	####
	pasttime=DF[1]									## from where we start
	Next = future(intensity,intensitySum) ## here simulations gives the next point and next type
	currenttime=pasttime+Next$elapsed
	
	while(currenttime<=DF[2])
	{
		points=c(points,currenttime)
		pointstype=c(pointstype, Next$type)
		
		Nxt=update(Next$elapsed,Next$type,psi, interac)
		
		psi=Nxt$psi
		intensity=Nxt$intensity
		intensitySum=Nxt$intensitySum
		
		pasttime=currenttime
	
		Next=future(intensity,intensitySum)	
		
		currenttime=pasttime+Next$elapsed
						
	}
	return(list(points=points,types=pointstype,psi=psi,intensity=intensity,intensitySum=intensitySum))	
}

Hawkesmulti<-function(Ntrial,delay,BoxComp,spontComp,interacComp)
{
	#### returns Data in the the shape of DataNeur with Ntrials
	### BoxComp is a vector of size Ntrial fil with cells, in each cell a matrix with 3 lines begin, end and comportment of each box, column= box
	#### spontComp matrix with Ntype lines and Ncomp column giving the spontaneous parameters
	#### interacComp array of cells dimension Ntype(lin), Ntype(col), Ncomp(comp) : in each cell a piecewise contant function (ie partition value of partition)  for h col->lin in comportement comp
	#### beginning of each trial we stay in behavior comp for a time = delay before starting the first box
	#### BoxComp should be disjoints and in increasing order for each trial
	
	#initialisation
	
	Ntype=nrow(spontComp)
	Ncomp=ncol(spontComp)
	
	# computation of R the largest dependance delay per comportment
	
	R=rep(0,Ncomp)   # initialisation
	
	for(comp in 1:Ncomp)
	{
		for(type in 1:Ntype)
		{
			for(i in 1:Ntype)
			{
				a			= interacComp[i,type,comp][[1]]
				R[comp]     = max(R[comp],a[1,ncol(a)])
	
				if(a[1,1]!=0)
				{
					print(paste('Error, first point of the partition',i,'<-',j,'not 0',sep=' '))
					break
				}
				if(prod(a[1,]==sort(a[1,]))==0)
				{
					print(paste('Error, partition',i,'<-',j,'not in increasing order',sep=' '))
					break
				}
				if(a[2,ncol(a)]!=0)
				{
					print(paste('Error, last value for the partition',i,'<-',j,'not zero',sep=' '))
					break
				}
			}	
		}
	}
	
	### initialization for the format DataNeur as a list
	
	Data		= array(data=c(list()),c(Ntype,Ntrial))   # initialization as a list

    nbpt=matrix(data=0,nrow=Ntype,ncol=Ntrial) # future maximum number of points

	
	###### the loop on the trials
	for(trial in 1:Ntrial)
	{
		Box	=BoxComp[[trial]]   #### the boxes on which we want to simulate
		nbox= ncol(Box)
		
		### the warm-up period
		
		DF	=c(Box[1,1]-delay, Box[1,1])
		comp=Box[3,1]
		
		### initialisation of psi, intensity and intensitySum
		
		intensitySum=matrix(c(0,sum(spontComp[,comp])),nrow=2)
		intensity   =lapply(1:Ntype,FUN=function(type)
					{
						return(matrix(c(0,spontComp[type,comp]),nrow=2))
					})
		psi=intensity			
		interac    =	interacComp[,,comp]
		if(is.array(interac)==FALSE){interac=array(c(interac),c(Nneur,Nneur))}
							
		H	= Hawkes(DF,psi,intensity,intensitySum,interac) ### the points that have warmed up and their intensity
		
		points=c()
		types=c()
		psi=H$psi
		intensity=H$intensity
		intensitySum=H$intensitySum
		lasttime=H$points[length(H$points)]
		lasttype=H$types[length(H$types)]
		if(length(lasttime)==0){lasttime=DF[2]}
		
		###passage du bon passe pas au point
		comppast=comp
				
		for(b in 1:nbox)
		{
			DF=c(Box[1,b],Box[2,b]) ### the box
			comp=Box[3,b] ### the comportment
			interac    =	interacComp[,,comp]
			if(is.array(interac)==FALSE){interac=array(c(interac),c(Nneur,Nneur))}
			
			if(comp==comppast)
			{
				elapsed=DF[1]-lasttime
				intensitySum=cutting(elapsed,intensitySum)
				intensity=lapply(1:Ntype,FUN=function(i){cutting(elapsed,intensity[[i]])})	
				psi=lapply(1:Ntype,FUN=function(i){cutting(elapsed,psi[[i]])})	
			}else
			{
				intensitySum=matrix(c(0,sum(spontComp[,comp])),nrow=2)     ### initialisation with the good spontananeous parameters
				intensity   =lapply(1:Ntype,FUN=function(type)
					{
						return(matrix(c(0,spontComp[type,comp]),nrow=2))
					})
				indexpast=which((DF[1]-points)<=R[comp]) ### indices of the points in the past that may influence what's happening after DF[1]
				li=length(indexpast)
				if(li!=0)
				{
					pt=points[indexpast]
					tp=types[indexpast]
					ptref=pt[1]
					for(p in 1:li)
					{
						elapsed=pt[p]-ptref
						type=tp[p]
						Nxt=update(elapsed,type,psi, interac) # we update with past simulated points
						psi=Nxt$psi
						intensity=Nxt$intensity
						intensitySum=Nxt$intensitySum					
						ptref=pt[p]
					}
					elapsed=DF[1]-pt[li] #### it remains to cut it to arrive at the eginning of the box
					intensitySum=cutting(elapsed,intensitySum)
					intensity=lapply(1:Ntype,FUN=function(i){cutting(elapsed,intensity[[i]])})
					psi=lapply(1:Ntype,FUN=function(i){cutting(elapsed,psi[[i]])})	
				}
			}
			
						
			H	= Hawkes(DF,psi,intensity,intensitySum,interac)
			
			points=c(points, H$points)
			types=c(types,H$types)
			psi=H$psi
			intensity=H$intensity
			intensitySum=H$intensitySum
			lasttime=H$points[length(H$points)]
			lasttype=H$types[length(H$types)]
		
			comppast=comp
					
		}
		
		### almost in the correct format
		
		if(length(points)==0){
			nbpt[,trial]=0
		}else
		{
			for (ntype in 1:Ntype)
        	{
        		pointneur		= points[which(types==ntype)]		# the pointss with type=ntype
        		nbpt[ntype,trial]=length(pointneur)					# their numbers
        		Data[ntype,trial][[1]]=pointneur	 				    # everything in a list
         	}
		}				
		
	}
	
	##### in the DataNeur object
	
	nmax= max(nbpt)
	DataNeur=matrix(data=0,nrow=Ntrial*Ntype,ncol=nmax+1)

	for (trial in 1:Ntrial)
	{
		for (type in 1:Ntype)
        {
        	index		= Ntrial*(type-1)+trial                  # indice correspondant dans Data
        	DataNeur[index,1] = nbpt[type,trial]                                     # on met le nombre de vrai points
        	if (DataNeur[index,1]>0)
			    {
        		DataNeur[index,2:(DataNeur[index,1]+1)] = Data[type,trial][[1]]
			    }   
        }
   } 
	return(DataNeur)
}
