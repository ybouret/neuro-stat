
source("test_init.R")

L=wink_single_H(neurons[[1]],neurons[[2]],0.1,0.1+delta,delta,B);

count_plus =sum(L$Hc>=L$H);
count_minus=sum(L$Hc<=L$H);

L2=wink_single_T(neurons[[1]],neurons[[2]],0.1,0.1+delta,delta,B);

count_plus2  = sum(L2$Tp>=L2$T);
count_minus2 = sum(L2$Tp<=L2$T);
