#include "wink-os.hpp"

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <cstdlib>

namespace wink 
{
    const char *platform_name() throw()
    {
        return WINK_PLATFORM;
    }
    
    void critical_error( int err, const char *txt ) throw()
    {
        fprintf(stderr,"*** critical error!!!\n");
        fprintf(stderr,"*** %s\n", txt);
        fprintf(stderr,"*** %s\n", strerror(err) );
        fprintf(stderr,"*** aborting !!");
        exit(1);
    }
}

