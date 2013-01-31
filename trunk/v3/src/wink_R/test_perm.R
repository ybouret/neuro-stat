source("test_init.R")

par(mfrow=c(2,2))
num_threads=4;

## Generate Permutation Statistic
print("---- Testing T ----");
#T/parallel
stamp=proc.time()
alpha_T_par = wink_permutations("T",neurons[[1]],neurons[[2]],intervals,delta,B,num_threads);
tmx_T_par =proc.time() - stamp;

print(tmx_T_par[3]);
plot(intervals[1,],alpha_T_par[1,],type="o",col="blue")
points(intervals[1,],alpha_T_par[2,],type="o",col="red")

#T/serial
stamp=proc.time();
alpha_T_ser = wink_permutations("T",neurons[[1]],neurons[[2]],intervals,delta,B,1);
tmx_T_ser = proc.time() - stamp;
print(tmx_T_ser[3]);
plot(intervals[1,],alpha_T_ser[1,],type="o",col="skyblue")
points(intervals[1,],alpha_T_ser[2,],type="o",col="orange")

alpha_diff = alpha_T_par - alpha_T_ser;
rms = norm(alpha_diff,"f")/prod(dim(alpha_diff));
print(paste("rms_T=",rms));
speedup = tmx_T_ser[[3]]/tmx_T_par[[3]];
print(paste("speedup_T=",speedup));
print("");

print("---- Testing H ----");
#H/parallel
stamp=proc.time()
alpha_H_par = wink_permutations("H",neurons[[1]],neurons[[2]],intervals,delta,B,num_threads);
tmx_H_par =proc.time() - stamp;

print(tmx_H_par[3]);
plot(intervals[1,],alpha_H_par[1,],type="o",col="blue")
points(intervals[1,],alpha_H_par[2,],type="o",col="red")

#H/serial
stamp=proc.time();
alpha_H_ser = wink_permutations("H",neurons[[1]],neurons[[2]],intervals,delta,B,1);
tmx_H_ser = proc.time() - stamp;
print(tmx_H_ser[3]);
plot(intervals[1,],alpha_H_ser[1,],type="o",col="skyblue")
points(intervals[1,],alpha_H_ser[2,],type="o",col="orange")

alpha_diff = alpha_H_par - alpha_H_ser;
rms = norm(alpha_diff,"f")/prod(dim(alpha_diff));
print(paste("rms_H=",rms));
speedup = tmx_H_ser[[3]]/tmx_H_par[[3]];
print(paste("speedup_H=",speedup));


