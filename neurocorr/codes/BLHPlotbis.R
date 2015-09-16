BLHPlotbis<-function(Lasso,Nneur,k,del,spontComp,interacComp,Lname=c())
{
	if(length(Lname)==0){Lname=as.character(c(1:Nneur))}
	Ncomp=length(Lasso)
	
	if (Nneur<=5)
	{
		for (comp in 1:Ncomp)
		{
			#file=paste("Neur",Nneur,"_Comp",comp,sep="")
			#postscript(file)
			#a=dev.cur()
			X11()
			par(mfrow=c(Nneur,Nneur))
			for (nneur1 in 1:Nneur)
			{
				for (nneur2 in 1:Nneur)
				{
					true=interacComp[nneur1,nneur2,comp][[1]] # the true interaction
					l=ncol(true)
					xT=rep(true[1,],each=2)
					yT=c(0,rep(true[2,1:l-1],each=2),0)
				
					# the Lasso reconstruction
					coeffs=Lasso[[comp]]$B[(k*(nneur2-1)+2):(k*nneur2+1),nneur1]
					values=c(coeffs/sqrt(del),0)
					part=c(0:k)*del
					x=rep(part,each=2)
					yB=c(0,rep(values[1:k],each=2),0)
				
					spontB=floor(Lasso[[comp]]$B[1,nneur1]*10)/10;
				
					# The Lasso +OLS reconstruction
					coeffOLS=Lasso[[comp]]$BO[(k*(nneur2-1)+2):(k*nneur2+1),nneur1]
					valueOLS=c(coeffOLS/sqrt(del),0)
					yBO=c(0,rep(valueOLS[1:k],each=2),0)
				
					spontBO=floor(Lasso[[comp]]$BO[1,nneur1]*10)/10;
				
					plot(xT,yT,type="l",xlim=c(0,k*del),ylim=c(min(c(yT,yB,yBO,0))*2,max(c(yT,yB,yBO,0))*2),main=paste(Lname[nneur2],'->',Lname[nneur1],"S*=",round(spontComp[nneur1,comp],3), " SB=",round(spontB,3)," SBO=", round(spontBO,3)),xlab="seconds",ylab="Hz")
				
					lines(x,yB,type="l",col='green')
					lines(x,yBO,type="l",col='red')
				

				}
			}
			#dev.off(a)
		}
	}
	else
	{
		if (Nneur<=10)
		{
			for (comp in 1:Ncomp)
			{
				file=paste("Neur",Nneur,"_Comp",comp,"_1-5","_1-5",sep="")
				postscript(file)
				a=dev.cur()
				par(mfrow=c(5,5))
				for (nneur1 in 1:5)
				{
					for (nneur2 in 1:5)
					{
						true=interacComp[nneur1,nneur2,comp][[1]] # the true interaction
						l=ncol(true)
						xT=rep(true[1,],each=2)
						yT=c(0,rep(true[2,1:l-1],each=2),0)
				
						# the Lasso reconstruction
						coeffs=Lasso[[comp]]$B[(k*(nneur2-1)+2):(k*nneur2+1),nneur1]
						values=c(coeffs/sqrt(del),0)
						part=c(0:k)*del
						x=rep(part,each=2)
						yB=c(0,rep(values[1:k],each=2),0)
				
						spontB=floor(Lasso[[comp]]$B[1,nneur1]*10)/10;
				
						# The Lasso +OLS reconstruction
						coeffOLS=Lasso[[comp]]$BO[(k*(nneur2-1)+2):(k*nneur2+1),nneur1]
						valueOLS=c(coeffOLS/sqrt(del),0)
						yBO=c(0,rep(valueOLS[1:k],each=2),0)
				
						spontBO=floor(Lasso[[comp]]$BO[1,nneur1]*10)/10;
				
						plot(xT,yT,type="l",xlim=c(0,k*del),ylim=c(min(c(yT,yB,yBO,0))*2,max(c(yT,yB,yBO,0))*2),main=paste(Lname[nneur2],'->',Lname[nneur1],"S*=",round(spontComp[nneur1,comp],3), " SB=",round(spontB,3)," SBO=", round(spontBO,3)),xlab="seconds",ylab="Hz")
				
						lines(x,yB,type="l",col='green')
						lines(x,yBO,type="l",col='red')
				

					}
				}
				dev.off(a)
				file=paste("Neur",Nneur,"_Comp",comp,"_1-5","_6-",Nneur,sep="")
				postscript(file)
				a=dev.cur()
				par(mfrow=c(5,(Nneur-5)))
				for (nneur1 in 1:5)
				{
					for (nneur2 in 6:Nneur)
					{
						true=interacComp[nneur1,nneur2,comp][[1]] # the true interaction
						l=ncol(true)
						xT=rep(true[1,],each=2)
						yT=c(0,rep(true[2,1:l-1],each=2),0)
				
						# the Lasso reconstruction
						coeffs=Lasso[[comp]]$B[(k*(nneur2-1)+2):(k*nneur2+1),nneur1]
						values=c(coeffs/sqrt(del),0)
						part=c(0:k)*del
						x=rep(part,each=2)
						yB=c(0,rep(values[1:k],each=2),0)
				
						spontB=floor(Lasso[[comp]]$B[1,nneur1]*10)/10;
				
						# The Lasso +OLS reconstruction
						coeffOLS=Lasso[[comp]]$BO[(k*(nneur2-1)+2):(k*nneur2+1),nneur1]
						valueOLS=c(coeffOLS/sqrt(del),0)
						yBO=c(0,rep(valueOLS[1:k],each=2),0)
				
						spontBO=floor(Lasso[[comp]]$BO[1,nneur1]*10)/10;
				
						plot(xT,yT,type="l",xlim=c(0,k*del),ylim=c(min(c(yT,yB,yBO,0))*2,max(c(yT,yB,yBO,0))*2),main=paste("S*=",spontComp[nneur1,comp], " SB=",spontB," SBO=", spontBO),xlab="seconds",ylab="Hz")
				
						lines(x,yB,type="l",col='green')
						lines(x,yBO,type="l",col='red')
				

					}
				}
				dev.off(a)
				file=paste("Neur",Nneur,"_Comp",comp,"_6-",Nneur,"_1-5",sep="")
				postscript(file)
				a=dev.cur()
				par(mfrow=c((Nneur-5),5))
				for (nneur1 in 6:Nneur)
				{
					for (nneur2 in 1:5)
					{
						true=interacComp[nneur1,nneur2,comp][[1]] # the true interaction
						l=ncol(true)
						xT=rep(true[1,],each=2)
						yT=c(0,rep(true[2,1:l-1],each=2),0)
				
						# the Lasso reconstruction
						coeffs=Lasso[[comp]]$B[(k*(nneur2-1)+2):(k*nneur2+1),nneur1]
						values=c(coeffs/sqrt(del),0)
						part=c(0:k)*del
						x=rep(part,each=2)
						yB=c(0,rep(values[1:k],each=2),0)
				
						spontB=floor(Lasso[[comp]]$B[1,nneur1]*10)/10;
				
						# The Lasso +OLS reconstruction
						coeffOLS=Lasso[[comp]]$BO[(k*(nneur2-1)+2):(k*nneur2+1),nneur1]
						valueOLS=c(coeffOLS/sqrt(del),0)
						yBO=c(0,rep(valueOLS[1:k],each=2),0)
				
						spontBO=floor(Lasso[[comp]]$BO[1,nneur1]*10)/10;
				
						plot(xT,yT,type="l",xlim=c(0,k*del),ylim=c(min(c(yT,yB,yBO,0))*2,max(c(yT,yB,yBO,0))*2),main=paste("S*=",spontComp[nneur1,comp], " SB=",spontB," SBO=", spontBO),xlab="seconds",ylab="Hz")
				
						lines(x,yB,type="l",col='green')
						lines(x,yBO,type="l",col='red')
				

					}
				}
				dev.off(a)
				file=paste("Neur",Nneur,"_Comp",comp,"_6-",Nneur,"_6-",Nneur,sep="")
				postscript(file)
				a=dev.cur()
				par(mfrow=c((Nneur-5),(Nneur-5)))
				for (nneur1 in 6:Nneur)
				{
					for (nneur2 in 6:Nneur)
					{
						true=interacComp[nneur1,nneur2,comp][[1]] # the true interaction
						l=ncol(true)
						xT=rep(true[1,],each=2)
						yT=c(0,rep(true[2,1:l-1],each=2),0)
				
						# the Lasso reconstruction
						coeffs=Lasso[[comp]]$B[(k*(nneur2-1)+2):(k*nneur2+1),nneur1]
						values=c(coeffs/sqrt(del),0)
						part=c(0:k)*del
						x=rep(part,each=2)
						yB=c(0,rep(values[1:k],each=2),0)
				
						spontB=floor(Lasso[[comp]]$B[1,nneur1]*10)/10;
				
						# The Lasso +OLS reconstruction
						coeffOLS=Lasso[[comp]]$BO[(k*(nneur2-1)+2):(k*nneur2+1),nneur1]
						valueOLS=c(coeffOLS/sqrt(del),0)
						yBO=c(0,rep(valueOLS[1:k],each=2),0)
				
						spontBO=floor(Lasso[[comp]]$BO[1,nneur1]*10)/10;
				
						plot(xT,yT,type="l",xlim=c(0,k*del),ylim=c(min(c(yT,yB,yBO,0))*2,max(c(yT,yB,yBO,0))*2),main=paste("S*=",spontComp[nneur1,comp], " SB=",spontB," SBO=", spontBO),xlab="seconds",ylab="Hz")
				
						lines(x,yB,type="l",col='green')
						lines(x,yBO,type="l",col='red')
				

					}
				}
				dev.off(a)


				
				
			}
		}
	}
	
}































