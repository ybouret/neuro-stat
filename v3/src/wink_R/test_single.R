
source("test_init.R")

par(mfrow=c(2,2))

window_width = 10 * delta;
L=wink_single_boot(neurons[[1]],neurons[[2]],0.1,0.1+window_width,delta,B);

count_plus =sum(L$Hc>=L$H);
count_minus=sum(L$Hc<=L$H);

plot.ecdf(L$Hc);

Ltau=wink_single_boot(neurons[[1]],neurons[[2]],0.1,0.1+window_width,delta,B,"tau");

count_plus_tau =sum(Ltau$Hc>=Ltau$H);
count_minus_tau=sum(Ltau$Hc<=Ltau$H);

plot.ecdf(Ltau$Hc);

LT=wink_single_perm("T",neurons[[1]],neurons[[2]],0.1,0.1+window_width,delta,B);

count_plus_T  = sum(LT$Sp>=LT$S);
count_minus_T = sum(LT$Sp<=LT$S);
plot.ecdf(LT$Sp);

LH=wink_single_perm("H",neurons[[1]],neurons[[2]],0.1,0.1+window_width,delta,B);

count_plus_H  = sum(LH$Sp>=LH$S);
count_minus_H = sum(LH$Sp<=LH$S);
plot.ecdf(LH$Sp);
