YOCTO_ROOT="./yocto"
CREATE="${YOCTO_ROOT}/src/share/create.sh"


all:

update:
	@sh ./setup.sh
	
clean:

veryclean: clean
	@echo "-- removing out of sources builds" && cd forge && touch targets && ( cat targets | xargs rm -rf ) && rm -f targets

xcode:
	@sh $(CREATE) src xcode ${BUILD_TYPE}
	
gnu:
	@sh $(CREATE) src gnu ${BUILD_TYPE}
	
