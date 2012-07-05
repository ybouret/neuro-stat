#include "wink-neuro-pair.hpp"
#include "wink.hpp"

namespace wink 
{
    
    neuro_pair:: ~neuro_pair() throw()
    {
    }
    
#define NEURO_PAIR_CTOR() \
boot( B <= 1 ? 1 : B ),   \
Bcoinc( boot.indx ),      \
Bcount( boot.size ),      \
true_coinc(0),            \
g()
    
    
    neuro_pair::neuro_pair(size_t        ntrials, 
                           const double *data1, 
                           size_t        ncol1, 
                           const double *data2, 
                           size_t        ncol2,
                           size_t        B) :
    N1( data1, ntrials, ncol1 ),
    N2( data2, ntrials, ncol2 ),
    perm( ntrials ),
    NEURO_PAIR_CTOR()
    {
        
    }
    
    static inline size_t __check_rows( size_t r1, size_t r2 )
    {
        if( r1 != r2 )
            throw "matrices doesn't have the same #rows"; 
        return r1;
    }
    
    neuro_pair:: neuro_pair( const c_matrix &M1, const c_matrix &M2, size_t B ) :
    N1( M1.data, M1.nrow, M1.ncol ),
    N2( M2.data, M2.nrow, M2.ncol ),
    perm( __check_rows(M1.nrow,M2.nrow) ),
    NEURO_PAIR_CTOR()
    {
    }
    
    
    
    
    double neuro_pair:: pvalue( double a, double b, double delta ) throw()
    {
        assert(a<=b);
        // prepare the windows
        N1.prepare_windows(a,b);
        N2.prepare_windows(a,b);
        
        // get the true coincidences
        true_coinc = wink::true_coincidences(N1, N2, delta, perm);
        
        // compute the bootstraped distribution
        permutation_bootstrap(Bcoinc, Bcount, N1, N2, delta, perm,g);
        
        // and evaluate the thingy
        return permutation_pvalue(true_coinc, Bcoinc, Bcount);
    }
    
    
}