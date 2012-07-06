
ifdef WINDIR
	DYN_EXT:=".dll"#
else
	DYN_EXT:=".so"#
endif

all:
	MAKEFLAGS='CXXFLAGS=-O2\ -Wall\ -DNDEBUG=1\ -pipe' R CMD SHLIB -c --preclean -o wink_ser$(DYN_EXT) ../wink/*.cpp wink_ser.cpp
	MAKEFLAGS='CXXFLAGS=-O2\ -Wall\ -DNDEBUG=1\ -pipe' R CMD SHLIB -c --preclean -o wink_par$(DYN_EXT) ../wink/*.cpp ../wink/par/*.cpp wink_par.cpp

clean:
	@echo "-- cleaning"
	rm -rf *.o *~

