
source("test_init.R")

par(mfrow=c(3,1))

window_width = 10 * delta;
L=wink_single_boot(neurons[[1]],neurons[[2]],0.1,0.1+window_width,delta,B);

count_plus =sum(L$Hc>=L$H);
count_minus=sum(L$Hc<=L$H);

plot.ecdf(L$Hc);

LT=wink_single_perm(neurons[[1]],neurons[[2]],0.1,0.1+window_width,delta,B,"T");

count_plus_T  = sum(LT$Sp>=LT$S);
count_minus_T = sum(LT$Sp<=LT$S);
plot.ecdf(LT$Sp);

LH=wink_single_perm(neurons[[1]],neurons[[2]],0.1,0.1+window_width,delta,B,"H");

count_plus_H  = sum(LH$Sp>=LH$S);
count_minus_H = sum(LH$Sp<=LH$S);
plot.ecdf(LH$Sp);
