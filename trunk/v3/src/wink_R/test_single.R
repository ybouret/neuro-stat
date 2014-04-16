
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

TS_H=wink_single_TS("H",neurons[[1]],neurons[[2]],0.1,0.1+window_width,delta,B);

count_plus_TS_H  = sum(TS_H$Sts>=TS_H$S);
count_minus_TS_H = sum(TS_H$Sts<=TS_H$S);
plot.ecdf(TS_H$Sts);

TS_T=wink_single_TS("T",neurons[[1]],neurons[[2]],0.1,0.1+window_width,delta,B);

count_plus_TS_T  = sum(TS_T$Sts>=TS_T$S);
count_minus_TS_T = sum(TS_T$Sts<=TS_T$S);
plot.ecdf(TS_T$Sts);

