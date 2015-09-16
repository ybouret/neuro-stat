ifdef WINDIR
	DYN_EXT:=".dll"#
else
	DYN_EXT:=".so"#
endif

C_SRCS := record.cpp neuron.cpp trial.cpp neurodata.cpp
LDFLAGS := -L../../yocto/sdk/lib/ -lyocto

all:
	MAKEFLAGS='CXXFLAGS=-O2\ -Wall\ -DNDEBUG=1\ -pipe' R CMD SHLIB -c --preclean -o neurocorr$(DYN_EXT) neurocorr.cpp $(C_SRCS) $(LDFLAGS)

default:
	R CMD SHLIB -c --preclean -o neurocorr$(DYN_EXT) neurocorr.cpp $(C_SRCS) $(LDFLAGS)

clean:
	rm -f neurocorr.so neurocorr.dll Rplots.pdf

