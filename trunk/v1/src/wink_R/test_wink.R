
## generate vectors of pvalues(>=)

source("test_init.R")

stamp = proc.time();
p0 <- bootstrap_pvalues_ser(neurons[[1]],neurons[[2]],intervals,delta,B);
ser_tmx = proc.time() - stamp;
print(ser_tmx[3]);

stamp = proc.time();
p1 <- bootstrap_pvalues_par(neurons[[1]],neurons[[2]],intervals,delta,B,4);
par_tmx = proc.time() - stamp;
print(par_tmx[3]);

print("-------- SPEEDUP --------");
print(ser_tmx[[3]]/par_tmx[[3]]);

