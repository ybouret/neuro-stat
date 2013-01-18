source("test_init.R")

par(mfrow=c(2,1))

## how to generate bootstrap with permutation
boot_perm <- wink_bootstrap(neurons[[1]],neurons[[2]],intervals,delta,B,"perm");
plot(intervals[1,],boot_perm[1,],type="o",col="blue")
points(intervals[1,],boot_perm[2,],type="o",col="red")
#readLines("stdin", n = 1)

## how to generate bootstrap with replacement
boot_repl <- wink_bootstrap(neurons[[1]],neurons[[2]],intervals,delta,B,"repl");

plot(intervals[1,],boot_repl[1,],type="o",col="blue")
points(intervals[1,],boot_repl[2,],type="o",col="red")
