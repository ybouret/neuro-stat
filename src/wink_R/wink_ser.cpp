#include <iostream>
#include <ctime>

#include <R.h>
#include <Rmath.h>
#include <Rinternals.h>

#include "../wink/wink-neuro-pair.hpp"


extern "C"
SEXP wink_ser( SEXP data1, SEXP data2, SEXP windows, SEXP Rdelta, SEXP RB) throw()
{
    wink::rand32_kiss g;
    g.seed(time(NULL));
	
      
#if 0
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
    const size_t  nw = INTEGER(RdimW)[1];
    const double *pw = REAL(windows);
    Rprintf("\t#### number of windows=%u\n",unsigned(nw));
    if( nw <= 0 )
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
        // make C++ neuro_pair
        //======================================================================
        wink::neuro_pair   NP(M1,M2,B);
        
        //======================================================================
        // create the return vector
        //======================================================================
        SEXP Rval;
        PROTECT(Rval = allocVector(REALSXP,nw) );
        double *ans = REAL(Rval);
        
        //======================================================================
        // outer loop on windows
        //======================================================================
        for( size_t i=0; i < nw; ++i )
        {
            //------------------------------------------------------------------
            // get the boundaries
            //------------------------------------------------------------------
            const double a = pw[0+2*i];
            const double b = pw[1+2*i];
            
            //------------------------------------------------------------------
            // call the integrated function
            //------------------------------------------------------------------
            const double pvalue = NP.pvalue(a,b,delta);
            //Rprintf("[%10.6f,%10.6f] :  true_coinc=%6u : pvalue= %.8f\n",a,b,unsigned(NP.true_coinc),pvalue);
            
            ans[i] = pvalue;
        }
        UNPROTECT(1);
        
        return Rval;
    }
    catch(...)
    {
        Rprintf("Exception in code");
    }
    return R_NilValue;
}
