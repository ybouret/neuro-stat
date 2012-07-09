
ifdef WINDIR
	DYN_EXT:=".dll"#
else
	DYN_EXT:=".so"#
endif

all:
	MAKEFLAGS='CXXFLAGS=-O2\ -Wall\ -DNDEBUG=1\ -pipe' R CMD SHLIB -c --preclean -o wink$(DYN_EXT) ../wink/*.cpp ../wink/par/*.cpp ./wink.cpp

clean:
	@echo "-- cleaning"
	rm -rf *.o *~

