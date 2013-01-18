source("test_init.R")

par(mfrow=c(2,1))



## how to generate bootstrap with permutation, parallel code
stamp=proc.time()
alpha_perm_par <- wink_bootstrap_par(neurons[[1]],neurons[[2]],intervals,delta,B,"perm",2);
perm_tmx_par=proc.time() - stamp;
print(perm_tmx_par[3]);

## how to generate bootstrap with permutation
stamp=proc.time()
alpha_perm <- wink_bootstrap(neurons[[1]],neurons[[2]],intervals,delta,B,"perm");
perm_tmx=proc.time() - stamp;
print(perm_tmx[3]);

alpha_diff = alpha_perm - alpha_perm_par;
rms = norm(alpha_diff,"f")/prod(dim(alpha_diff));
print(paste("rms=",rms));
speedup = perm_tmx[[3]]/perm_tmx_par[[3]];
print(paste("speedup=",speedup));

