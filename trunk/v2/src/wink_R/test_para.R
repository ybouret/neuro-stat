source("test_init.R")


par(mfrow=c(2,2))

num_threads=4;

## how to generate permutation T or H, parallel code
stamp=proc.time()
alpha_perm_par <- wink_permutation_par(neurons[[1]],neurons[[2]],intervals,delta,B,"T",num_threads);
perm_tmx_par=proc.time() - stamp;
print(perm_tmx_par[3]);
plot(intervals[1,],alpha_perm_par[1,],type="o",col="blue")
points(intervals[1,],alpha_perm_par[2,],type="o",col="red")


## how to generate permutation T or H, serial code
stamp=proc.time()
alpha_perm <- wink_permutation(neurons[[1]],neurons[[2]],intervals,delta,B,"T");
perm_tmx=proc.time() - stamp;
print(perm_tmx[3]);
plot(intervals[1,],alpha_perm_par[1,],type="o",col="blue")
points(intervals[1,],alpha_perm_par[2,],type="o",col="red")

alpha_diff = alpha_perm - alpha_perm_par;
rms = norm(alpha_diff,"f")/prod(dim(alpha_diff));
print(paste("rms=",rms));
speedup = perm_tmx[[3]]/perm_tmx_par[[3]];
print(paste("speedup_T=",speedup));

stamp=proc.time()
alpha_perm_par <- wink_permutation_par(neurons[[1]],neurons[[2]],intervals,delta,B,"H",num_threads);
perm_tmx_par=proc.time() - stamp;
print(perm_tmx_par[3]);
plot(intervals[1,],alpha_perm_par[1,],type="o",col="skyblue")
points(intervals[1,],alpha_perm_par[2,],type="o",col="orange")

stamp=proc.time()
alpha_perm <- wink_permutation(neurons[[1]],neurons[[2]],intervals,delta,B,"H");
perm_tmx=proc.time() - stamp;
print(perm_tmx[3]);
plot(intervals[1,],alpha_perm_par[1,],type="o",col="skyblue")
points(intervals[1,],alpha_perm_par[2,],type="o",col="orange")

alpha_diff = alpha_perm - alpha_perm_par;
rms = norm(alpha_diff,"f")/prod(dim(alpha_diff));
print(paste("rms=",rms));
speedup = perm_tmx[[3]]/perm_tmx_par[[3]];
print(paste("speedup_H=",speedup));

