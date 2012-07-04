#include <R.h>
#include <Rmath.h>
#include <Rinternals.h>

#include "../wink/wink.hpp"

extern "C"
SEXP demo( SEXP data1, SEXP data2 ) throw()
{
#if 0
    wink::init_alea();
    GetRNGstate();
    for( size_t i=0; i < 10; ++i )
    {
        Rprintf("%g\n", unif_rand());
    }
    PutRNGstate();
#endif

    //==========================================================================
    // extract parameters from data1
    //==========================================================================
    SEXP Rdim1 = getAttrib(data1, R_DimSymbol);
    const size_t  nrow1 = INTEGER(Rdim1)[0];
    const size_t  ncol1 = INTEGER(Rdim1)[1];
    Rprintf("N1: nrow=%u, ncol=%u\n", unsigned(nrow1), unsigned(ncol1) );
    
    //==========================================================================
    // extract parameters from data2
    //==========================================================================
    SEXP Rdim2 = getAttrib(data2, R_DimSymbol);
    const size_t  nrow2 = INTEGER(Rdim2)[0];
    const size_t  ncol2 = INTEGER(Rdim2)[1];
    Rprintf("N2: nrow=%u, ncol=%u\n", unsigned(nrow2), unsigned(ncol2) );
    
    if( nrow1 != nrow2 )
    {
        Rprintf("Error: different #trials\n");
        return R_NilValue;
    }
    
    try 
    {
        //======================================================================
        // transform R data intro C++ objects
        //======================================================================
        wink::c_matrix     M1(nrow1,ncol1);
        wink::c_matrix     M2(nrow2,ncol2);
        
        M1.loadR( REAL(data1) );
        M2.loadR( REAL(data2) );
        
        //======================================================================
        // make C++ neuro_trials
        //======================================================================
        wink::neuro_trials N1(M1.data,nrow1,ncol1);
        wink::neuro_trials N2(M2.data,nrow2,ncol2);
        N1.display();
        N2.display();
    }
    catch(...)
    {
        Rprintf("Exception in code");
    }
    return R_NilValue;
}