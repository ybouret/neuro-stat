#!bash
########################################################################
## Display error and exit
########################################################################
function xerror
{
    cmake -E cmake_echo_color --red --bold "-- $1";
    exit 1;
}

if (( $# < 1 )); then
	xerror "Need Debug or Release";
fi
cmake -E cmake_echo_color --magenta "-- Building $1 Configuration";
 
case $1 in
	"Debug"|"Release") ;;
	*) xerror "Invalid Configuration!";
esac;


YOCTO_ROOT="yocto"
cmake -E cmake_echo_color --blue --bold "-- Checking yocto sources";

# is source code here ?
if [ -d $YOCTO_ROOT ]; then
	cmake -E cmake_echo_color --black  "-- yocto directory is present";
	(cd $YOCTO_ROOT && svn up) || xerror "could't update yocto code!";
else
	cmake -E cmake_echo_color --black  "-- yocto directory not found..";
	cmake -E cmake_echo_color --cyan   "-- fetching code by SVN";
	svn co https://github.com/ybouret/yocto4/trunk $YOCTO_ROOT || xerror "couldn't fetch yocto code!";
fi

#(re)building SDK
cmake -E cmake_echo_color --blue --bold "-- Building yocto SDK...";
(cd $YOCTO_ROOT && $SHELL build-main+r.sh gnu $1) || xerror "couldn't build yocto SDK!";

