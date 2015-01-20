
source("test_init.R")

## how to generate true coincidences...
true_coinc_T <- wink_true_coincidences("T",neurons[[1]],neurons[[2]],intervals,delta);
print(true_coinc_T)

true_coinc_H <- wink_true_coincidences("H",neurons[[1]],neurons[[2]],intervals,delta);
print(true_coinc_H)


