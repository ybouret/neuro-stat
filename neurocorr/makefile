YOCTO_ROOT="./yocto"
CREATE="${YOCTO_ROOT}/src/share/create.sh"


all:

update_svn: update
	svn up

update_git: update
	git pull

update:
	@bash ./setup.sh
	
clean:
	@$(MAKE) -C src/v1 clean

veryclean: clean
	@echo "-- removing out of sources builds" && cd forge && touch targets && ( cat targets | xargs rm -rf ) && rm -f targets

xcode:
	@bash $(CREATE) src xcode ${BUILD_TYPE}
	
gnu:
	@bash $(CREATE) src gnu ${BUILD_TYPE}
	
clang:
	@bash $(CREATE) src clang ${BUILD_TYPE}

