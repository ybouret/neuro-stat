source("test_init.R")

par(mfrow=c(2,1))
num_threads=4;

# Generate Bootstrap

#parallel
stamp=proc.time();
counts_par = wink_bootstrap_counts(neurons[[1]],neurons[[2]],intervals,delta,B,num_threads,"tau");
par_tmx=proc.time() - stamp;

alpha_par = (counts_par+1)/(B+1);

print(par_tmx[3]);
plot(intervals[1,],alpha_par[1,],type="o",col="blue")
points(intervals[1,],alpha_par[2,],type="o",col="red")


stamp=proc.time();
counts_ser = wink_bootstrap_counts(neurons[[1]],neurons[[2]],intervals,delta,B,1,"tau");
ser_tmx=proc.time() - stamp;

alpha_ser = (counts_ser+1)/(B+1);

print(ser_tmx[3]);
plot(intervals[1,],alpha_ser[1,],type="o",col="skyblue")
points(intervals[1,],alpha_ser[2,],type="o",col="orange")

alpha_diff = alpha_par - alpha_ser;
rms = norm(alpha_diff,"f")/prod(dim(alpha_diff));
print(paste("rms_boot=",rms));
speedup = ser_tmx[[3]]/par_tmx[[3]];
print(paste("speedup_boot=",speedup));

