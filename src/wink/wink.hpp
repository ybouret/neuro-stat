//! file
/**
 Wink Is a NeuroStats Kernel.
 */
#ifndef WINK_INCLUDED
#define WINK_INCLUDED 1

#include <cstdlib>

namespace wink
{
    //! init a built in uniform random number generator in ]0:1[
    void   init_alea();
    
    //! returns a uniform random number generator in ]0:1[
    double alea();
    
    size_t random_index( double (*uniform_generator)() = wink::alea );
    
    //! sort by increasing order
    /**
     \param x an array of reals
     \param n x[0..n-1]
     */
    void   sort_array( double *x, size_t n );
   
    //! sort by increasing order
    /**
     \param x an array of indices
     \param n x[0..n-1]
     */
    void   sort_indices( size_t *x, size_t n );

    
    
    //! fill array with alea value
    void   fill_alea_array( double *x, size_t n, double a, double b );
    
    //! detect coincidences
    /**
     \param X an array of ordered time values
     \param Nx X[0..Nx-1]
     \param Y an array of ordered time values
     \param Ny Y[0..Ny-1]
     \param delta the time lag for detection
     \return the number of |X_i-Y_j| <= delta
     */
    size_t coincidences(const double *X, 
                        const size_t  Nx, 
                        const double *Y, 
                        const size_t  Ny, 
                        const double  delta);
    
    
    //! find index such that  X[i-1] < a <= X[i]
    /**
     \param a any real
     \param X a record X[0] = Nx, X[1..Nx] are the ORDERED data
     \param i the index
     \return true if this is possible and i is the answer, false otherwise and i=0
     */
    bool find_index_after( double a, const double *X, size_t &i );
    
    //! find index such that  X[i] <= b < X[i+1]
    /**
     \param b any real
     \param X a record X[0] = Nx, X[1..Nx] are the ORDERED data
     \param i the index
     \return true if this is possible and i is the answer, false otherwise and i=0
     */
    bool find_index_before( double b, const double *X, size_t &i );
    
    //! location of data
    class iwindow
    {
    public:
        iwindow() throw();
        ~iwindow() throw();
        size_t indexA; //!< first index
        size_t indexB; //!< last  index
        size_t length; //!< indexB - indexA  +1, 0 is no valid data
        
        //! get indices from data
        /**
         for length to be >0, find_index_after(a,X,indexA) must succeed
         and find_index_before(b,X,indexB) must succeed as weel.
         if( indexA > indexB ), this means that [a,b] is
         between two consecutive X, so the window is empty.
         */
        void   initialize( double a, double b, const double *X );
    private:
        iwindow( const iwindow & );
        iwindow & operator=( const iwindow & );
    };
    
    
    //! detect coincidences
    /**
     \param X a record X[0] = Nx, X[1..Nx] are the ORDERED data
     \param Wx a valid window associated to X
     \param Y a record Y[0] = Ny, Y[1..Ny] are the ORDERED data
     \param Wy a valid window associated to Y
     \return the number of coincidences within lag delta
     */
    size_t coincidences(const double   *X, 
                        const iwindow  &Wx,
                        const double   *Y,
                        const iwindow  &Wy,
                        const double    delta);
    
    //! build a random permutation
    /**
     \param indices will contained the permutted indices
     \param num indices[0..num]
     \param uniform_generator in [0:1]
     */
    void build_permutation( size_t *indices, size_t num, double (*uniform_generator)() );
    
    //! build an identity vector
    void build_identity( size_t *indices, size_t num );
    
    //! holds memory for a permutation
    class permutation
    {
    public:
        permutation( size_t n );
        ~permutation() throw();
        
        const size_t size;
        size_t      *indices;
        
        void identity() throw();
        void rebuild( double (*uniform_generator)() ) throw();
        
    private:
        permutation( const permutation & );
        permutation&operator=(const permutation & );
    };
    
    
    
    class neuro_trials
    {
    public:
        neuro_trials( const double *data, const size_t nr, const size_t nc );
        ~neuro_trials() throw();
        
        class record
        {
        public:
            const double *X; //!< one trial X[0]=NX, X[1..Nx]
            iwindow       W; //!< associated window
            record() throw();
            ~record() throw();
            
        private:
            record( const record & );
            record &operator=(const record & );
        };
        
        const size_t num;  //!< #num records = nr
        const size_t top;  //!< max valid index: nc-1 
        record      *rec;  //!< array of num records
        
        void prepare_windows( double a, double b ) throw();
        
        
        void display() const;
        
    private:
        neuro_trials( const neuro_trials & );
        neuro_trials&operator=(const neuro_trials & );
    };
    
    
    //! total coincidences on a prepared window.
    /**
     The windows must have been prepared before: 
     N1.prepare_windows(a,b);
     N2.prepare_windows(a,b);
     */
    size_t total_coincidences(const neuro_trials &N1, 
                              const neuro_trials &N2, 
                              const double       delta,
                              const permutation &perm ) throw();
    
    
    //! true coincidences on a prepared window
    /**
     call perm.identity() and total_coincidences(N1,N2,perm,delta);
     */
    size_t true_coincidences(const neuro_trials &N1, 
                             const neuro_trials &N2, 
                             const double       delta,
                             permutation       &perm) throw();
    
    //! bootstrap coincidences on a prepared window
    /**
     call perm.rebuild(uniform_generator)
     and total_coincidences(N1,N2,perm,delta);
     */
    size_t bootstrap_coincidences(const neuro_trials &N1, 
                                  const neuro_trials &N2, 
                                  const double       delta,
                                  permutation       &perm,
                                  double            (*uniform_generator)()) throw();
    
    //! create a bootstrap sample
    /**
     \param Bcoinc coincidences
     \param Bcount Bcoinc[0..Bcount-1]
     */
    void permutation_bootstrap(size_t             *Bcoinc, 
                               const size_t        Bcount, 
                               const neuro_trials &N1, 
                               const neuro_trials &N2, 
                               const double       delta,
                               permutation       &perm,
                               double            (*uniform_generator)()) throw();
    
    //! find the pvalue from a sample and a true coincidence
    double permutation_pvalue( const size_t true_coinc, const size_t *Bcoinc, const size_t Bcount ) throw();
    
}

#endif
