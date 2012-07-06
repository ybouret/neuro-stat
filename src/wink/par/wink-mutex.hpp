//! \file

#ifndef WINK_MUTEX_INCLUDED
#define WINK_MUTEX_INCLUDED 1

#include "../wink-os.hpp"

#if defined(WINK_BSD)
#	if !defined(__USE_UNIX98)
#		define __USE_UNIX98
#	endif
#	include <pthread.h>
#endif  

#if defined(WINK_WIN)
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#endif /* YOCTO_WIN */

namespace wink 
{
    
    class Mutex
    {
    public:
        explicit Mutex() throw();
        virtual ~Mutex() throw();
        
        void lock() throw();
        void unlock() throw();
        
    private:
        Mutex(const Mutex&);
        Mutex&operator=(const Mutex & );
        
#if	defined(WINK_BSD)
        pthread_mutex_t mutex_;
#endif
        
#if	defined(WINK_WIN)
        CRITICAL_SECTION mutex_;
#endif

    };
 
    class ScopeLock
    {
    public:
        ScopeLock( Mutex &mutex ) throw();
        ~ScopeLock() throw();
        
    private:
        Mutex &host;
        ScopeLock(const ScopeLock & );
        ScopeLock &operator=( const ScopeLock & );
    };
    
    
}

#endif
