
source("test_init.R")

L=wink_single_H(neurons[[1]],neurons[[2]],0.1,0.1+delta,delta,B);

count_plus =sum(L$Hc>=L$H);
count_minus=sum(L$Hc<=L$H);
