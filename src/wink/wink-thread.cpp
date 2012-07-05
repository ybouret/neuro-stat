#include "wink-thread.hpp"
#include <assert.h>
#include <cerrno>

namespace wink
{
    
#if defined(WINK_WIN)
    DWORD WINAPI thread::launch( LPVOID args ) throw()
    {
        thread *thr= static_cast<thread *>(args);
        assert( thr );
        thr->proc_( thr->data_ );		
        return 0;
    }
    
#endif
    
#if defined(WINK_BSD)
    void * thread::launch( void *args ) throw()
    {
        thread *thr= static_cast<thread *>(args);
        assert( thr );
        thr->proc_( thr->data_ );
        return NULL;
    }
#endif
    
    
    
    
    thread:: thread( thread::proc_t proc, void *data ) :
    proc_( proc ),
    data_( data ),
#if defined(WINK_WIN)
    id32_(0),
#endif
    handle_()
    {
#if	defined(WINK_BSD)
        const int res = pthread_create( &handle_, NULL, thread::launch, this);
        if( res != 0 )
            throw "pthread_create failure";
#endif
        
#if defined(WINK_WIN)
        handle_ = ::CreateThread(0 ,
                                 0 ,
                                 thread::launch,
                                 this,
                                 0,
                                 & id32_ );
        if( NULL == handle_ )
        {
            throw "CreateThread Failure"; 
        }
        
#endif
    }
    
    
    void thread:: join() throw()
    {
#if defined(WINK_BSD)
        const int res = pthread_join( handle_, 0 );
        if( res != 0 )
        {
            critical_error( res, "pthread_join" );
        }
#endif
        
#if defined(WINK_WIN)
        //WINK_GIANT_LOCK();
        if( ::WaitForSingleObject( handle_ , INFINITE ) != WAIT_OBJECT_0 )
        {
            critical_error( EINVAL, "WaitForSingleObject" );
        }
#endif
    }
    
    
    thread:: ~thread() throw()
    {
#if defined(WINK_BSD)
        
#endif
        
#if defined(WINK_WIN)
        ::CloseHandle( handle_ );
#endif
    }
    
    thread::id_t thread::get_id() const throw()
    {
#if defined(WINK_BSD)
        return handle_;
#endif
        
#if defined(WINK_WIN)
        return id32_;
#endif
    }
    
    thread::handle_t thread:: get_handle() const throw()
    {
        return handle_;
    }
    
    thread::handle_t thread:: get_current_handle() throw()
    {
#if defined(WINK_BSD)
        return pthread_self();
#endif
        
#if defined(WINK_WIN)
        return ::GetCurrentThread();
#endif
        
    }
    
    thread::id_t thread:: get_current_id() throw()
    {
#if defined(WINK_BSD)
        return pthread_self();
#endif
        
#if defined(WINK_WIN)
        return ::GetCurrentThreadId();
#endif
    }
    
}
