########################################################################
## Display error and exit
########################################################################
function xerror
{
    cmake -E cmake_echo_color --red --bold "-- $1";
    exit 1;
}

YOCTO_ROOT="yocto"
cmake -E cmake_echo_color --blue --bold "-- Checking yocto sources";

# is source code here ?
if [ -d $YOCTO_ROOT ]; then
	cmake -E cmake_echo_color --black  "-- yocto directory is present";
else
	cmake -E cmake_echo_color --black  "-- yocto directory not found..";
	cmake -E cmake_echo_color --cyan   "-- fetching code by SVN";
	svn co https://github.com/ybouret/yocto4/trunk $YOCTO_ROOT || xerror "couldn't fetch yocto code!";
fi

#is sdk ready ?
YOCTO_SDK="$YOCTO_ROOT/sdk"
cmake -E cmake_echo_color --blue --bold "-- Checking yocto SDK...";
if [ -d $YOCTO_SDK ]; then
	cmake -E cmake_echo_color --black "-- found SDK";
else
	cmake -E cmake_echo_color --black "-- need to build SDK...";
	(cd yocto && $SHELL build-main+r.sh gnu Debug);
fi
