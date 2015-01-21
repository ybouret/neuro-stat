source("test_init.R");

t0     = proc.time();
p_seq  = pValues(intervals,delta,neurons[[1]],neurons[[2]],"symetric");
t_seq  = proc.time()-t0;

t0     = proc.time();
p_par1 = pValuesPara(intervals,delta,neurons[[1]],neurons[[2]],"symetric",1);
t1     = proc.time() - t0;

t0     = proc.time();
p_par2 = pValuesPara(intervals,delta,neurons[[1]],neurons[[2]],"symetric",2);
t2     = proc.time() - t0;

t0     = proc.time();
p_par3 = pValuesPara(intervals,delta,neurons[[1]],neurons[[2]],"symetric",3);
t3     = proc.time() - t0;

t0     = proc.time();
p_par4 = pValuesPara(intervals,delta,neurons[[1]],neurons[[2]],"symetric",4);
t4     = proc.time() - t0;

nprocs = c(0,1,2,3,4);
tindex = 3;
tmx    = c(t_seq[[tindex]],t1[[tindex]],t2[[tindex]],t3[[tindex]],t4[[tindex]]);

