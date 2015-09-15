

all:

fetch_yocto:
	@if [ -d "yocto4" ]; then     \
		echo "found yocto4"; \
	else \
	  	echo "installing yocto4"; \
		svn co https://github.com/ybouret/yocto4/trunk yocto4; \
	fi

build_yocto:
	@cd yocto4 && sh build-main+r.sh
	
clean:


