source("test_init.R")

#H  <- wink_true_coincidences(neurons[[1]],neurons[[2]],intervals,delta,"H");
HC <- wink_bootstrap_counts(neurons[[1]],neurons[[2]],intervals,delta,B);

## pvalues are computed by  by (HC)/(B) or (HC+1)/(B+1)
