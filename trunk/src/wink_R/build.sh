MAKEFLAGS='CXXFLAGS=-O2\ -Wall\ -DNDEBUG=1\ -pipe' R CMD SHLIB -c --preclean -o wink_ser.so ../wink/*.cpp wink_ser.cpp -O2 

