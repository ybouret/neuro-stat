source("test_init.R")
par(mfrow=c(1,1))

H  <- wink_true_coincidences(neurons[[1]],neurons[[2]],intervals,delta,"H");
HC <- wink_bootstrap_counts(neurons[[1]],neurons[[2]],intervals,delta,B);

## pvalues are computed by  by (HC)/(B) or (HC+1)/(B+1)
alpha0 = HC/B;
alpha1 = (HC+1)/(B+1);

plot(intervals[1,],alpha0[1,],type="o",col="blue")
points(intervals[1,],alpha0[2,],type="o",col="red")

plot(intervals[1,],alpha1[1,],type="o",col="skyblue")
points(intervals[1,],alpha1[2,],type="o",col="orange")

