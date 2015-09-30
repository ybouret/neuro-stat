ifdef WINDIR
	DYN_EXT:=".dll"#
else
	DYN_EXT:=".so"#
endif

all:
	MAKEFLAGS='CXXFLAGS=-O2\ -Wall\ -DNDEBUG=1\ -pipe' R CMD SHLIB -c --preclean -o wink$(DYN_EXT) ../wink/*.cpp ../pyck/*.cpp wink.cpp

default:
	R CMD SHLIB -c --preclean -o wink$(DYN_EXT) ../wink/*.cpp ../pyck/*.cpp wink.cpp

clean:
	rm -f wink.so wink.dll Rplots.pdf

