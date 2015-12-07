#Fetching with SVN
In your work directory 'WORKDIR'
- to get the main code for the first time, execute
"svn co https://github.com/ybouret/neuro-stat/trunk neuro-stat"
- to update the main code, got to 'WORKDIR/neurostat' and execute
"svn up"

# Building the current NeuroCorr R extension for Unix-like systems (Linux/MacOsX/BSD...)
- go to 'neurocorr'
- execute "make update" to fetch and build the extra code
- got to 'neurocorr/v4'
- execute "make" : copy the 'neurocorr.so' and 'neurocorr.R' into your working directory.

#Running the code from the R command line
- 'neurocorr.R' and ('neurocorr.so' or 'neurocorr.dll') MUST be in the working directory. In R,execute source("neurocorr.R") to use the functions.

#Running the code from a R graphical interface
- Using 'R-app' in MacOsX, you need to change the working directory before sourcing the code.

