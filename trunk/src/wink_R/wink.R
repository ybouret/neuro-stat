
dyn.load("wink.so")
source("simulationPP.R")
neurons <- simulPP(0,5,1,5);
neurons[[1]]
.Call("demo", neurons[[1]], neurons[[2]] );



