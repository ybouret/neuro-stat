CREATE=./share/create.sh

all:

clean:
	@echo "-- removing MacOSX Finder info" && find . -name '.DS_Store' | xargs rm -f
	@echo "-- removing .dat, .ppm, .vtk, .png, .bin  and .dot" && rm -f *.dat *.ppm *.vtk *.png *.dot *.bin
	@echo "-- cleaning auxiliary files"

preclean:
	@echo "-- removing out of sources builds" && cd forge && touch targets &&  ( ( cat targets | xargs rm -rf ) && rm -f targets )

veryclean: preclean clean

xcode:
	@bash ${CREATE} src xcode ${BUILD_TYPE} 
	
vs9:
	@bash ${CREATE} src vs9 ${BUILD_TYPE}

vs10:
	@bash ${CREATE} src vs10 ${BUILD_TYPE}


gnu:
	@bash ${CREATE} src gnu ${BUILD_TYPE}

clang:
	@bash ${CREATE} src clang ${BUILD_TYPE}

intel:
	@bash ${CREATE} src intel ${BUILD_TYPE}

codeblocks:
	@bash ${CREATE} src codeblocks ${BUILD_TYPE}
	
