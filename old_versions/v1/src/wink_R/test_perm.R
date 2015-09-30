
source("wink.R")

## fills a matrix with num_rows
## each row is a random permutation of seq(1:perm_size)
wink_perm_matrix <- function( num_rows, perm_size )
{
	if(perm_size<=0)
		stop("perm_size must be >0 ")
		
	Mp <- matrix( data=0, nrow=num_rows, ncol=perm_size );
	for( i in 1:nrow(Mp) )
	{
		Mp[i,] <- wink_perm( ncol(Mp) )
	}
	return(Mp)
}
