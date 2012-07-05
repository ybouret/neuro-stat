#include "wink-mutex.hpp"

#if defined(WINK_BSD)
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#endif

namespace wink 
{
    
    ScopeLock:: ScopeLock( Mutex &mutex ) throw() :
    host( mutex )
    {
        host.lock();
    }
    
    ScopeLock:: ~ScopeLock() throw()
    {
        host.unlock();
    }
    
#if defined(WINK_BSD)
    static inline
    void critical_error( int err, const char *txt )
    {
        fprintf(stderr,"*** critical error!!!\n");
        fprintf(stderr,"*** %s\n", txt);
        fprintf(stderr,"*** %s\n", strerror(err) );
        fprintf(stderr,"*** aborting !!");
        exit(1);
    }
#endif
    
    Mutex:: Mutex() throw() :
    mutex_()
    {
        //-- initialize the mutex
#if defined(WINK_WIN)
        ::InitializeCriticalSection( & mutex_ );
#endif
        
#if defined(WINK_BSD)
        {
            pthread_mutexattr_t attr;
            
            int  res = pthread_mutexattr_init( &attr );
            if (res != 0)
                critical_error(res, "pthread_mutexattr_init");
            
            res = pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_RECURSIVE);
            if (res != 0)
            {
                pthread_mutexattr_destroy( &attr );
                critical_error(res, "pthread_mutexattr_settype(RECURSIVE)");
            }
            
            res = pthread_mutex_init( & mutex_ , &attr );
            (void) pthread_mutexattr_destroy( &attr );
            if( res != 0 )
                critical_error(res,"pthread_mutex_init");
        }
#endif        
    }
    
    Mutex:: ~Mutex() throw()
    {
#if defined(WINK_WIN)
        ::DeleteCriticalSection( & mutex_ );
#endif
        
#if defined(WINK_BSD)
        {
            const int res = pthread_mutex_destroy( &mutex_ );
            if( res != 0 )
                critical_error( res, "pthread_mutex_destroy" );
        }
#endif
    }
    
    void Mutex:: lock() throw()
    {
#if defined(WINK_WIN)
        ::EnterCriticalSection(  & mutex_  );
#endif
        
#if defined(WINK_BSD)
        const int res = pthread_mutex_lock( &mutex_ );
        if( res != 0 )
            critical_error( res,  "pthread_mutex_lock" );
#endif

    }
    
    
    void Mutex:: unlock() throw()
    {
#if defined(WINK_WIN)
        ::LeaveCriticalSection( &mutex_  );
#endif
        
#if defined(WINK_BSD)
        const int res = pthread_mutex_unlock( & mutex_ );
        if( res != 0 )
            critical_error( res,  "pthread_mutex_lock" );
#endif

    }
}