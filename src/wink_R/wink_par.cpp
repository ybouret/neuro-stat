#include "../wink/wink-neuro-pair.hpp"
#include "../wink/par/wink-worker.hpp"

#include <iostream>
#include <ctime>

#include <R.h>
#include <Rmath.h>
#include <Rinternals.h>

extern "C"
SEXP wink_par( SEXP data1, SEXP data2, SEXP windows, SEXP Rdelta, SEXP RB, SEXP Rnt) throw()
{
    
    //==========================================================================
    // extract parameters from data1
    //==========================================================================
    SEXP Rdim1 = getAttrib(data1, R_DimSymbol);
    const size_t  nrow1 = INTEGER(Rdim1)[0];
    const size_t  ncol1 = INTEGER(Rdim1)[1];
    Rprintf("\t#### N1: nrow=%u, ncol=%u\n", unsigned(nrow1), unsigned(ncol1) );
    
    //==========================================================================
    // extract parameters from data2
    //==========================================================================
    SEXP Rdim2 = getAttrib(data2, R_DimSymbol);
    const size_t  nrow2 = INTEGER(Rdim2)[0];
    const size_t  ncol2 = INTEGER(Rdim2)[1];
    Rprintf("\t#### N2: nrow=%u, ncol=%u\n", unsigned(nrow2), unsigned(ncol2) );
    
    
    if( nrow1 != nrow2 )
    {
        Rprintf("*** Error: different #trials\n");
        return R_NilValue;
    }
    
    //==========================================================================
    // extract parameters from windows
    //==========================================================================
    SEXP RdimW = getAttrib(windows, R_DimSymbol);
    if( 2 != INTEGER(RdimW)[0] )
    {
        Rprintf("*** Error: windows must have 2 rows");
    }
    const size_t  nwindows = INTEGER(RdimW)[1];
    const double *pwindows = REAL(windows);
    Rprintf("\t#### number of windows=%u\n",unsigned(nwindows));
    if( nwindows <= 0 )
    {
        Rprintf("*** Error: invalid #windows\n");
        return R_NilValue;
    }
    //==========================================================================
    // Get delta value
    //==========================================================================
    Rdelta = coerceVector(Rdelta,REALSXP);
    const double delta = REAL(Rdelta)[0];
    Rprintf("\t#### delta=%g\n",delta);
    
    //==========================================================================
    // Get B Count
    //==========================================================================
    RB = coerceVector(RB, INTSXP);
    const size_t B  = INTEGER(RB)[0];
    Rprintf("\t#### B=%u\n",unsigned(B));
    if( B <= 0 )
    {
        Rprintf("*** Error: invalid B<=0\n");
        return R_NilValue;
    }
    
    //==========================================================================
    // Get num_threads
    //==========================================================================
    Rnt = coerceVector(Rnt,INTSXP);
    size_t num_threads = INTEGER(Rnt)[0];
    Rprintf("\t#### num_threads=%u\n",unsigned(num_threads));
    if(num_threads<=0)
        num_threads = 1;
    if(num_threads>nwindows)
        num_threads = nwindows;
    
    
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
        // create the return vector
        //======================================================================
        SEXP Rval;
        PROTECT(Rval = allocVector(REALSXP,nwindows) );
        double *pvalues = REAL(Rval);

        //======================================================================
        // create the team of threads
        //======================================================================
        wink::workers team(M1,M2,B,num_threads,nwindows,pwindows,pvalues,delta);
        
        //======================================================================
        // wait for threads to complete
        //======================================================================
        team.wait();
        
        
        UNPROTECT(1);
        return Rval;
    }
    catch(...)
    {
        Rprintf("Exception in code");
    }
    
    return R_NilValue;
}
