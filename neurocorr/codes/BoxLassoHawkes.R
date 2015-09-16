BoxLassoHawkes<-function(DataNeur,Nneur,BoxEst,k,del,gamma)
# this function provide a list of two Lasso estimates of the coefficients per comportment: the classical one "a_B" and the one with an additional OLS step to make it unbiased "a_BO". It also gives the value of the Lasso criterion "crit" at a_B.
#The procedure is the Bernstein Lasso described in the paper "Lasso and probabilistic ineqaulities for multivariate point processes" by Hansen, Reynaud-Bouret and Rivoirard, but we authorize in this program the record to be non stationary as soon as boxes with prescribed comportments are given in BoxEst
# DataNeur : DataNeur is a matrix.  
###each line corresponds to 1 trial for 1 neuron, first column = number of points of 1 trial for 1 neuron, other columns = the (strictly positive) points then 0 to complete the line 
### in line first all the trials for the first neuron then all the trials for the second etc 
# Nneur : number of neurons
# BoxEst : repartition of the comportments for each trial, each cell corresponds to a trial, in each cell a matrix with four lines : first: ntrial, second: beginning of the box, third:  end of the box, fourth: comportment  number. Here the boxes do not need to be contiguous 
#k  number of intervals for reconstruction in the interaction functions
#del  length of each interval
# gamma = 1  tuning parameter. Mathematically, one should always take gamma>=1. In practice 1 or 2 seem fine. If gamma is large, less coefficients will be non zero.
{

######### Ntrial can be guessed
Ntrial=nrow(DataNeur)/Nneur	

######### Computations of the vector b and the matrix G as described in the paper for each comportment

# computation of the piecewise constant functions 1/sqrt(del)* \int \indic_{(i-1)del, i*del}(t-u)dN^ntrial_nneur(u), so 5 indexes: partition / value of partition, points/value, i, nneur, ntrial

phi = Nij(DataNeur, Ntrial, Nneur, k,del) 

# computation of the products of those functions 3 indexes : 1st : size=k*Nneur correspond to a couple (nneur1,i1), 2nd : size=k*Nneurcorrespond to a couple (nneur2,i2), 3rd : ntrial. For each choice of ntrial, (nneur1,i1), (nneur2,i2), compute the product of the previous functions, put the result as a matrix with two lines top : partition/ bottom: value of partition in the cell inside the array with three indexes

prod	= prodN(phi,Ntrial,Nneur,k) # The piecewise constants functions representing psi(phi1)*psi(phi2)

# computation of small G for each box c, put inside a vector of cells, each cell corresponding to a box, the "big G" of the article being a diagonal matrix by block formed of Nneur blocks of the "small G" computed here
# Since the computation are already localised in time in the variable phi and prod,  we do not need any contiguity between the boxes. Case just gives the boundary on which we integrate those functions.

Case = as.matrix(BoxEst[1:3,]) # remove the line corresponding to the comportment
G	 = Xtc(phi,prod,k,Nneur,Case) # G[1,c][[1]] is a small G for box c, should be a square matrix of size 1+k*Nneur


# computation of the vector b of the article : again there is one for each box c, but it also depends on the neuron nneur on which we integrate, so a matrix of cells line = nneur, column= box c, inside a vector of dimension 1+k*Nneur

Vb		= vb(DataNeur,Case,phi)

########### Computations of B and Vhat for the threshold 

### B has the same structure as G but contains the upperbound on the box c of the \psi^(m)_t(\phi) which only depends on i and ell
# inside a cell a vector of dimension 1+k*Nneur

Nc  = ncol(Case)
B	= array(data=c(list()),c(1,Nc))


for (c in 1:Nc)
{
	e=Case[1,c]  # number of trial
	btemp = matrix(data=0,ncol=1,nrow=(1+k*Nneur))
	btemp[1,1]=1  # for the spontaneous apparition
	r=2 # the current index
	for (ell in 1:Nneur)
	{
		for (i in 1:k)
		{
			ind=which((phi[1,,i,ell,e]>Case[2,c])&(phi[1,,i,ell,e]<=Case[3,c])) # portion in the right place
			if (length(ind)!=0)
			{
				ind=c(ind[1]-1,ind) # plus the value at the beginning
				btemp[r,1]=max(phi[2,ind,i,ell,e]) # the maximum
			}
			else
			{
				ind=which(phi[1,,i,ell,e]<=Case[2,c]) # index of the  points in the partition before the box
				btemp[r,1]=phi[2,max(ind),i,ell,e] # value at the last point before the box
			}
			r=r+1				 	 	
		}
	}
	B[1,c][[1]]=btemp
}
	
	
### Vhat has the same structure as Vb, it contains the optional bracket of the martingale. Inside a cell : a vector of dimension 1+k*Nneur

Vhat=array(data=c(list()),c(Nneur,Nc))

for (n in 1:Nneur)
{
	for (c in 1:Nc)
	{
		e=Case[1,c]  # number of trial
		vtemp = matrix(data=0,ncol=1,nrow=(1+k*Nneur))
		ind= which((DataNeur[Ntrial*(n-1)+e,]>Case[2,c])&(DataNeur[Ntrial*(n-1)+e,]<=Case[3,c])) # which indexes have the points of neuron n in trial e in the box c
		if (length(ind)!=0)  # if no point it's 0
		{
			vtemp[1,1]=length(ind)  # for the spontaneous apparition, total number of points in the box
			r=2
			for (ell in 1:Nneur)
			{
				for (i in 1:k)
				{
					vtemp[r,1]=intijnc(DataNeur[Ntrial*(n-1)+e,ind], prod[(r-1),(r-1),e][[1]])
					r=r+1
				}
			}
			
		}
		Vhat[n,c][[1]]=vtemp
	}
}

######## Computation of the different quantities by summing the boxes with the correct behavior... Each structure is first a list on the comportment, then inside a matrix or vector. For Vhat and Vb each column of the matrix corresponds to a neuron
  
####for G, do the sum over the c with the correct behavior
#FAIRE ATTENTION SI COMPORTEMENT VIDE
Ncomp=max(BoxEst[4,]) # number of comportments

Gcomp=lapply(1:Ncomp,FUN=function(comp)
		{
			ind=which(BoxEst[4,]==comp)
			if (length(ind)!=0)
			{
				return(Reduce('+',G[1,ind]))
			}
			else
			{
				return('no box with this behavior')
			}
		})
			
		
		
#####for Vb, same thing but an extra index due to nneur

Vbcomp=lapply(1:Ncomp,FUN=function(comp)
		{
			ind=which(BoxEst[4,]==comp)
			if (length(ind)!=0)
			{	
				temp=sapply(1:Nneur,FUN=function(nneur)
					{
						return(Reduce('+', Vb[nneur,ind]))
					})
				return(temp)
			}
			else
			{
				return('no box with this behavior')
			}

		})
		
####for Vhat, same thing as Vb

Vhatcomp=lapply(1:Ncomp,FUN=function(comp)
		{
			ind=which(BoxEst[4,]==comp)
			if (length(ind)!=0)
			{
				temp=sapply(1:Nneur,FUN=function(nneur)
					{
						return(Reduce('+', Vhat[nneur,ind]))
					})
				return(temp)
			}
			else
			{
				return('no box with this behavior')
			}	
		})	
		
##### for B need of a max term by term ...	

Bcomp=lapply(1:Ncomp,FUN=function(comp)
		{
			ind=which(BoxEst[4,]==comp)
			if (length(ind)!=0)
			{
				return(Reduce(function(u,v){
							w=cbind(u,v)  
							return(as.matrix(apply(w,1,max)))
						}, B[1,ind]))
			}
			else
			{
				return('no box with this behavior')
			}	
		})
		
####Hence the threshold

Thresh=lapply(1:Ncomp,FUN=function(comp)
		{
			ind=which(BoxEst[4,]==comp)
			if (length(ind)!=0)
			{
				Ttot=sum(BoxEst[3,ind]-BoxEst[2,ind])
				temp=sapply(1:Nneur,FUN=function(nneur)
					{
						return(sqrt(2*gamma*max(log(Ttot),1)*as.matrix(Vhatcomp[[comp]][,nneur]))+gamma*max(log(Ttot),1)*Bcomp[[comp]]/3)
					})
				return(temp)
			}
			else
			{
				return('no box with this behavior')
			}
		})	

#### and the Lasso solution


Lasso=lapply(1:Ncomp,FUN=function(comp)
		{   
			ind=which(BoxEst[4,]==comp)
			if (length(ind)!=0)
			{
				BL=sapply(1:Nneur, FUN=function(nneur)
					{
						Gd=diag(c(1/(Thresh[[comp]][,nneur])))%*%Gcomp[[comp]]%*%diag(c(1/(Thresh[[comp]][,nneur])))	 # transform the problem in order to apply lassoshooting
						shoot=lassoshooting(lambda=2,XtX=Gd,Xty=((1/(Thresh[[comp]][,nneur]))*Vbcomp[[comp]][,nneur])) # solution of lassoshooting
						return((shoot$coefficients)*as.matrix(1/(Thresh[[comp]][,nneur]))) # transformation to get our Lasso-Bernstein solution
					})
				BOL=sapply(1:Nneur, FUN=function(nneur)
					{
						poscoeff=which(abs(BL[,nneur])>0) # determination of the support	
						a_BO=0*as.matrix(BL[,nneur])
						if (length(poscoeff)!=0)
						{
							Gpos=Gcomp[[comp]][poscoeff,poscoeff] # G restricted to the support
				            a_BO[poscoeff]=solve(Gpos)%*%as.matrix(Vbcomp[[comp]][poscoeff,nneur]) #inversion ie OLS solution on the support
						}
						return(a_BO)
					})
				Crit=-2*sum(BL*Vbcomp[[comp]])+sum(BL*(Gcomp[[comp]]%*%BL))+2*sum(abs(BL)*Thresh[[comp]])
				return(list(B=BL,BO=BOL,PC=Crit))			
			}
			else
			{
				return('no box with this behavior')
			}	
		})
		
return(Lasso)	
	
}
