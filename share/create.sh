# !bash

########################################################################
## Display error and exit
########################################################################
function xerror
{
    cmake -E cmake_echo_color --red --bold "-- $1";
    exit 1;
}

cmake -E cmake_echo_color --blue "-- Initialize Build System...";

########################################################################
## Check proper location
########################################################################
[ -d ./forge ] || xerror "Missing ./forge"

case $# in
  2) BUILD_TYPE=Debug;;
  3) BUILD_TYPE=$3;;
  *) xerror "Usage is $0 SourceDir BuildTools [BuildType=Debug]" ;;
esac

BUILD_SOURCE=$1
[ -f $BUILD_SOURCE/CMakeLists.txt ] || xerror "Missing Top Level CMakeLists.txt";


########################################################################
## Check Build Type
########################################################################
case $BUILD_TYPE in
  "Debug" | "Release") ;;
  *) xerror "Invalid BuildType <$BUILD_TYPE>";;
esac

########################################################################
## Check Build Tools
########################################################################
BUILD_TOOLS=$2

if [ -z "$WINDIR" ];
then
  cmake -E cmake_echo_color --magenta "-- NOT building on Windows"
  WITH_MAKEFILES="Unix Makefiles"
else
  cmake -E cmake_echo_color --magenta "-- building on Windows"
  WITH_MAKEFILES="MSYS Makefiles"
fi

case $BUILD_TOOLS in
  "gnu" )
    export CC=gcc$VERSION; export CXX=g++$VERSION;
    BUILD_SUBDIR=gnu$VERSION/$BUILD_TYPE;
    BUILD_GENERATOR="$WITH_MAKEFILES"
    cmake -E cmake_echo_color --blue "-- CC=$CC | CXX=$CXX";
    ;;
    
  "clang" )
    export CC=clang$VERSION; export CXX=clang++$VERSION;
    BUILD_SUBDIR=clang$VERSION/$BUILD_TYPE;
    BUILD_GENERATOR="$WITH_MAKEFILES"
    cmake -E cmake_echo_color --blue "-- CC=$CC | CXX=$CXX";
    ;;
    
  "intel" )
      export CC=icc; export CXX=icpc;
      BUILD_SUBDIR=intel/$BUILD_TYPE;
      BUILD_GENERATOR="$WITH_MAKEFILES"
      cmake -E cmake_echo_color --blue "-- CC=$CC | CXX=$CXX";
    ;;
    
  "pathscale" )
      export CC=pacthcc; export CXX=pathCC;
      BUILD_SUBDIR=pathscale/$BUILD_TYPE;
      BUILD_GENERATOR="$WITH_MAKEFILES"
      cmake -E cmake_echo_color --blue "-- CC=$CC | CXX=$CXX";
    ;;
    
  "codeblocks") 
    BUILD_SUBDIR=cb/$BUILD_TYPE;
    BUILD_GENERATOR="CodeBlocks - $WITH_MAKEFILES";
    ;;
    
  "xcode")
    BUILD_SUBDIR=xcode;
    BUILD_GENERATOR="Xcode"
    ;;
    
  "vs9" )
    BUILD_SUBDIR=vs9;
    BUILD_GENERATOR="Visual Studio 9 2008";
    ;;

	"vs10" )
    BUILD_SUBDIR=vs10;
    BUILD_GENERATOR="Visual Studio 10";
    ;;

    
  *) xerror "Unsuported BuildTools <$BUILD_TOOLS>";;
esac

BUILD_SYSTEM=`uname -s`
case $BUILD_SYSTEM in
  "FreeBSD" | "Linux" | "SunOS" ) BUILD_TARGET=$BUILD_SYSTEM-`uname -m`;;
  *) BUILD_TARGET=$BUILD_SYSTEM;;
esac

cmake -E cmake_echo_color --blue "-- BUILD_SOURCE = <$BUILD_SOURCE>"
cmake -E cmake_echo_color --blue "-- BUILD_TOOLS  = <$BUILD_TOOLS>"
cmake -E cmake_echo_color --blue "-- BUILD_TYPE   = <$BUILD_TYPE>"
cmake -E cmake_echo_color --blue "-- BUILD_SYSTEM = <$BUILD_SYSTEM>"
cmake -E cmake_echo_color --blue "-- BUILD_TARGET = <$BUILD_TARGET>"

BUILD_ROOT=./forge/$BUILD_TARGET/$BUILD_SUBDIR
cmake -E cmake_echo_color "-- creating $BUILD_ROOT";
cmake -E make_directory $BUILD_ROOT || xerror "Cant' create $BUILD_ROOT";
case $BUILD_TOOLS in
  "xcode" | "vs9"  | "vs10") 
    BUILD_UP=../../..
    BUILD_OPT=""
    ;;
  *) 
    BUILD_UP=../../../..
    BUILD_OPT=-DCMAKE_BUILD_TYPE:STRING="$BUILD_TYPE";
    ;;
esac

########################################################################
## register the BUILD_TARGET
########################################################################
cmake -E cmake_echo_color --green --bold "-- registering $BUILD_TARGET"
cmake -E touch ./forge/targets || xerror "-- can't create targets file"
(grep $BUILD_TARGET ./forge/targets > /dev/null ) || (echo $BUILD_TARGET >> ./forge/targets)

########################################################################
## configuring
########################################################################
cmake -E cmake_echo_color --blue --bold "-- Configuring......"
cd $BUILD_ROOT && \
( cmake "$BUILD_UP/$BUILD_SOURCE" \
-G"$BUILD_GENERATOR" $BUILD_OPT   \
-DMY_CFLAGS="${MY_CFLAGS}"        \
-DMY_CXXFLAGS="${MY_CXXFLAGS}"    \
-DMY_LDFLAGS="${MY_LDFLAGS}"      \
|| xerror "CMake Failure!")

JLEVEL=""
case `uname -s` in
	"Linux")
		NPROCS=`grep ^processor /proc/cpuinfo | wc -l`;
		;;
		
	"FreeBSD" | "Darwin")
		NPROCS=`sysctl hw.ncpu | cut -d ' ' -f 2`
		;;
		
	"MINGW32_NT-5.1")
		NPROCS=`env | grep NUMBER_OF_PROCESSORS | cut -d '=' -f 2`;
		;;

	"SunOS" )
		NPROCS=`psrinfo -p`;
		;;
		
	*)
		NPROCS=1;
	;;
esac
cmake -E cmake_echo_color --magenta "-- #CPU=$NPROCS";
if (( $NPROCS > 8 )); then
	NPROCS=8;
fi
JLEVEL="-j${NPROCS}"
cmake -E cmake_echo_color --magenta "-- using $JLEVEL";

################################################################################
## Executing Remaining Commands: WE ARE in the BUILD_DIR !!
################################################################################
[ -z "$TARGETS" ] && cmake -E cmake_echo_color --blue --bold "-- Configuring is now done" && exit 0;

cmake -E cmake_echo_color --blue --bold  "-- Executing $TARGETS";


function xtarget
{
    tgt=$1;
    echo "-- executing [$1]";
    case $BUILD_TOOLS in
        "gnu" | "intel" | "path" | "clang"  )
            make $JLEVEL -s $tgt || xerror "can't build [$1]"
        ;;

        "xcode")
            [ "all" == "$tgt" ] && tgt="ALL_BUILD";
            (xcodebuild -parallelizeTargets -target $tgt -configuration $BUILD_TYPE 2>&1 | grep -v -E 'setenv|Phase|echo|^make|^Check|^cd|^Depend' ) || xerror "can't build $1"
        ;;

        "codeblocks")
          codeblocks --no-log --build --target=$tgt *.cbp;
        ;;

        "vs9" | "vs10" )
          [ "all" == "$tgt" ] && tgt="ALL_BUILD";
          cmake --build . --target $tgt --config $BUILD_TYPE || xerror "can't build [$1]";
        ;;

        *)
            xerror "can't directly build '$1' with '$BUILD_TOOLS'";
        ;;
    esac
}

for t in $TARGETS; do
    xtarget $t;
done;
