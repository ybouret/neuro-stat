#ifndef WINK_WORKER_INCLUDED
#define WINK_WORKER_INCLUDED 1

#include "../wink-neuro-pair.hpp"
#include "wink-thread.hpp"

namespace wink 
{
    //! data to compute on a slice of windows
    class slice
    {
    public:
        explicit slice(size_t        s_offset, 
                       size_t        s_length,
                       const double *ab,
                       double       *p,
                       const double  lag,
                       const uint32_t s32
                       ) throw();
        virtual ~slice() throw();
        
        const size_t   offset;
        const size_t   length;
        const double  *windows;
        double        *pvalues;
        const double   delta;
        const uint32_t seed;
    
    private:
        slice( const slice &);
        slice&operator=(const slice & );
    };
    
    enum compute_pvalues
    {
        compute_pvalues_geq,
        compute_pvalues_both
    };
    
    //! one neuro pair to act on a slice of windows
    class worker : public neuro_pair, public slice
    {
    public:
        explicit worker(const c_matrix &M1,
                        const c_matrix &M2, 
                        const size_t    B,
                        size_t          s_offset, 
                        size_t          s_length,
                        const double   *ab,
                        double         *p,
                        const double    lag,
                        const uint32_t  s32,
                        wink::mutex    &synchro,
                        compute_pvalues pvalues_kind
                        );
        
        virtual ~worker() throw();
        
        mutex                &access; //!< to synchronize is necessary
        const compute_pvalues which;  //!< single/both
        thread                thr;    //!< thread in which the computation is carried out
        
    private:
        worker( const worker & );
        worker&operator=( const worker & );
        static void engine( void * ) throw();
        void run() throw();
        
    };
    
    //! array of worker
    class workers
    {
    public:
        explicit workers(const c_matrix &M1,
                         const c_matrix &M2, 
                         const size_t    B,
                         const size_t    num_threads,
                         const size_t    num_windows,
                         const double   *ab,
                         double         *p,
                         const double    lag,
                         compute_pvalues pvalues_kind);
        
        virtual ~workers() throw();
        
        const size_t maxi;   //!< num_threads
        size_t       size;   //!< online
        worker      *crew;   //!< members
        mutex        access; //!< if need synchro       
        
        void wait() throw();
        
    private:
        workers( const workers & );
        workers & operator=( const workers & );
        void kill() throw();
        
    };
    
}

#endif
