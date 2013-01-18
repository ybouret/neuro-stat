source("test_init.R")

par(mfrow=c(2,1))

## how to generate bootstrap with permutation
stamp=proc.time()
alpha_perm <- wink_bootstrap(neurons[[1]],neurons[[2]],intervals,delta,B,"perm");
perm_tmx=proc.time() - stamp;
print(perm_tmx[3]);

plot(intervals[1,],alpha_perm[1,],type="o",col="blue")
points(intervals[1,],alpha_perm[2,],type="o",col="red")
#readLines("stdin", n = 1)

## how to generate bootstrap with replacement
stamp=proc.time()
alpha_repl <- wink_bootstrap(neurons[[1]],neurons[[2]],intervals,delta,B,"repl");
repl_tmx=proc.time() - stamp
print(repl_tmx[3]);

plot(intervals[1,],alpha_repl[1,],type="o",col="blue")
points(intervals[1,],alpha_repl[2,],type="o",col="red")
