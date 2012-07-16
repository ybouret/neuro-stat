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
    
    
    //! O/S thread MUTual EXclusion
    class mutex
    {
    public:
        explicit mutex() throw();
        virtual ~mutex() throw();
        
        void lock() throw();
        void unlock() throw();
        
    private:
        mutex(const mutex&);
        mutex&operator=(const mutex & );
        
#if	defined(WINK_BSD)
        pthread_mutex_t mutex_;
#endif
        
#if	defined(WINK_WIN)
        CRITICAL_SECTION mutex_;
#endif

    };
 
    //! lock a mutex for the current scope
    class scope_lock
    {
    public:
        scope_lock( mutex &m ) throw();
        ~scope_lock() throw();
        
    private:
        mutex &host;
        scope_lock(const scope_lock & );
        scope_lock &operator=( const scope_lock & );
    };
    
    
}

#endif
