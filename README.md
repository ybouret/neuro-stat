#Fetching with SVN
In a build directory 'BUILDIR'
- to get the main code for the first time, execute
"svn co https://github.com/ybouret/neuro-stat/trunk neuro-stat"
- to update the main code, got to 'BUILDIR/neurostat' and execute
"svn up"

# Building the current NeuroCorr R extension for Unix-like systems (Linux/MacOsX/BSD...)
- go to 'neurocorr'
- execute "make update" to fetch and build the extra code
- got to 'neurocorr/v4'
- execute "make" : copy the 'neurocorr.so' and 'neurocorr.R' into your working directory.

# Getting the code for Windows and the 32 bits version of R
- got to 'neurocorr'
- copy 'i386/neurocorr.dll' and 'v4/neurocorr.R' into your working directory

#Running the code from the R command line
- 'neurocorr.R' and ('neurocorr.so' or 'neurocorr.dll') MUST be in the working directory. In R,execute source("neurocorr.R") to use the functions.

#Running the code from a R graphical interface
- Using 'R-app' in MacOsX, you need to change the working directory before sourcing the code.
- Using 'Rgui' (32-bits) in Windows: you need to open a command terminal ('cmd.exe'), go to your working directory, and call Rgui with the its full path, a.k.a somehting like "C:\Program Files\R\R-3.2.2\bin\i386\Rgui.exe"

