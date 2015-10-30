if [ $# != 2 ]; then
	cmake -E cmake_echo_color --red "build.sh Tools BUILD_TYPE=(Debug|Release)";
	exit 1;	
fi
cmake -E cmake_echo_color --blue "Building SDK for $1, BUILD_TYPE=$2..."
TARGETS=install make $1 BUILD_TYPE=$2
