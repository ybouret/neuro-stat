MAKEFLAGS='CXXFLAGS=-O2\ -Wall\ -DNDEBUG=1\ -pipe' R CMD SHLIB -c --preclean wink.so ../wink/*.cpp -O2 -s

