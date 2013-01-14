
## generate matrices of pvalues
## first row: pvalues(>=)
## second row: pvalues (<=)

source("test_init.R")

stamp = proc.time();
mp0 <- bootstrap_both_pvalues_ser(neurons[[1]],neurons[[2]],intervals,delta,B);
ser_tmx = proc.time() - stamp;
print(ser_tmx[3]);

stamp = proc.time();
mp1 <- bootstrap_both_pvalues_par(neurons[[1]],neurons[[2]],intervals,delta,B,4);
par_tmx = proc.time() - stamp;
print(par_tmx[3]);

print("-------- SPEEDUP --------");
print(ser_tmx[[3]]/par_tmx[[3]]);

