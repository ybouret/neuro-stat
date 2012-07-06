#ifndef WINK_THREAD_INCLUDED
#define WINK_THREAD_INCLUDED 1

#include "wink-mutex.hpp"

namespace wink 
{
    class thread 
    {	
    public:
#if defined(WINK_BSD)
        typedef pthread_t handle_t;
        typedef pthread_t id_t;
#endif
        
#if defined(WINK_WIN)
        typedef HANDLE handle_t;
        typedef DWORD  id_t;
#endif
        
        typedef void (*proc_t)( void *);
        
        //! create and start a thread
        /**
         \param proc the called routine: proc(data)
         \param data the parameters: MUST BE PERSISTENT !
         */
        explicit thread( thread::proc_t proc, void *data);
        void     join()   throw();
        virtual ~thread() throw(); //!< must be joined before !
        
        id_t     get_id() const throw();
        handle_t get_handle() const throw();
		
        static id_t     get_current_id() throw();
        static handle_t get_current_handle() throw();
        
        static void assign_cpu( thread::handle_t , size_t cpu_id );
        void on_cpu( size_t cpu_id );
        
    private:
        thread( const thread & );
        thread&operator=(const thread &);
        
        proc_t       proc_;
        void        *data_;
#if defined(WINK_WIN)
        id_t         id32_;
#endif
        handle_t     handle_;
        
#if defined(WINK_BSD)
        static void * launch( void * ) throw();
#endif
        
#if defined(WINK_WIN)
        static DWORD WINAPI launch( LPVOID ) throw();
#endif
    };

    
}

#endif
