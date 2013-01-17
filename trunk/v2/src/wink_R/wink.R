########################################################################
##
## loading DLL
##
########################################################################


## build the DLL name
wink_dll <- paste("wink", .Platform$dynlib.ext, sep="")
print(paste("WINK: Loading",wink_dll) )

## load the dll only once
if( is.loaded("wink_version") )
	dyn.unload(wink_dll)
dyn.load(wink_dll)
if( !is.loaded("wink_version") )
	stop("WINK: Unable to find wink_version!")
