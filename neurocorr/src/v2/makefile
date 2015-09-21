ifdef WINDIR
	DYN_EXT:=".dll"#
else
	DYN_EXT:=".so"#
endif

C_SRCS  := types.cpp train.cpp neuron.cpp
LDFLAGS := -L../../yocto/sdk/lib/ -lyocto
R_TEST  := R --no-save --slave -f neurocorr.R

all:
	@MAKEFLAGS='CXXFLAGS=-O2\ -Wall\ -DNDEBUG=1\ -pipe' R CMD SHLIB -c --preclean -o neurocorr$(DYN_EXT) neurocorr.cpp $(C_SRCS) $(LDFLAGS)
	@$(R_TEST)

default:
	@R CMD SHLIB -c --preclean -o neurocorr$(DYN_EXT) neurocorr.cpp $(C_SRCS) $(LDFLAGS)
	@$(R_TEST)

clean:
	@rm -f neurocorr.so neurocorr.dll Rplots.pdf

