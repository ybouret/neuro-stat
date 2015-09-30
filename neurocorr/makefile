YOCTO_ROOT="./yocto"
CREATE="${YOCTO_ROOT}/src/share/create.sh"

ifndef BUILD_TYPE
BUILD_TYPE="Debug"#
endif

all:

update_svn: update
	svn up

update_git: update
	git pull

update:
	@echo "-- updating yocto"
	@bash ./setup.sh ${BUILD_TYPE}
	
clean:
	@echo "-- removing temporary files" && rm -f *.dat
	@$(MAKE) -s -C doc    clean
	@$(MAKE) -s -C v2 clean

veryclean: clean
	@echo "-- removing out of sources builds" && cd forge && touch targets && ( cat targets | xargs rm -rf ) && rm -f targets

xcode:
	@bash $(CREATE) . xcode ${BUILD_TYPE}
	
gnu:
	@bash $(CREATE) . gnu ${BUILD_TYPE}
	
clang:
	@bash $(CREATE) . clang ${BUILD_TYPE}

codeblocks:
	@bash $(CREATE) . codeblocks ${BUILD_TYPE}


