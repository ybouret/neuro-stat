source("test_init.R")
par(mfrow=c(1,1))

stamp=proc.time()
HC_par <- wink_bootstrap_counts_par(neurons[[1]],neurons[[2]],intervals,delta,B,4);
boot_tmx_par=proc.time() - stamp;
print(boot_tmx_par[3]);

alpha1_par = (HC_par+1)/(B+1);


plot(intervals[1,],alpha1_par[1,],type="o",col="blue")
points(intervals[1,],alpha1_par[2,],type="o",col="red")

stamp=proc.time();
HC <- wink_bootstrap_counts(neurons[[1]],neurons[[2]],intervals,delta,B);
boot_tmx=proc.time() - stamp;
print(boot_tmx[3]);

## pvalues are computed by  by (HC)/(B) or (HC+1)/(B+1)
alpha1 = (HC+1)/(B+1);


plot(intervals[1,],alpha1[1,],type="o",col="skyblue")
points(intervals[1,],alpha1[2,],type="o",col="orange")

speedup = boot_tmx[[3]]/boot_tmx_par[[3]];
print(paste("bootstrap_speedup=",speedup));

alpha_diff = alpha1 - alpha1_par;
rms = norm(alpha_diff,"f")/prod(dim(alpha_diff));
print(paste("rms=",rms));
