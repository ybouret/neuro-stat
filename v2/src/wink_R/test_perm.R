source("test_init.R")

par(mfrow=c(1,1))

## how to generate bootstrap with permutation
stamp=proc.time()
alpha_T <- wink_permutation(neurons[[1]],neurons[[2]],intervals,delta,B,"T");
perm_tmx=proc.time() - stamp;
print(perm_tmx[3]);

plot(intervals[1,],alpha_T[1,],type="o",col="blue")
points(intervals[1,],alpha_T[2,],type="o",col="red")
#readLines("stdin", n = 1)

## how to generate bootstrap with replacement
stamp=proc.time()
alpha_H <- wink_permutation(neurons[[1]],neurons[[2]],intervals,delta,B,"H");
repl_tmx=proc.time() - stamp
print(repl_tmx[3]);

plot(intervals[1,],alpha_H[1,],type="o",col="blue")
points(intervals[1,],alpha_H[2,],type="o",col="red")
