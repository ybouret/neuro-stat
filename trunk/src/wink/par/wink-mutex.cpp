#include "wink-mutex.hpp"



namespace wink 
{
    
    scope_lock:: scope_lock( mutex &m ) throw() :
    host( m )
    {
        host.lock();
    }
    
    scope_lock:: ~scope_lock() throw()
    {
        host.unlock();
    }
    

    
    mutex:: mutex() throw() :
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
    
    mutex:: ~mutex() throw()
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
    
    void mutex:: lock() throw()
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
    
    
    void mutex:: unlock() throw()
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