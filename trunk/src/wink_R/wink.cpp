#include "../wink/wink-neuro-pair.hpp"
#include "../wink/par/wink-worker.hpp"

#include <iostream>
#include <ctime>

#include <R.h>
#include <Rmath.h>
#include <Rinternals.h>

namespace 
{
    
    ////////////////////////////////////////////////////////////////////////////
    //
    // common parameters for functions
    //
    ////////////////////////////////////////////////////////////////////////////
    struct parameters
    {
        size_t nrow1;
        size_t ncol1;
        
        size_t nrow2;
        size_t ncol2;
        
        size_t        num_windows;
        const double *ptr_windows;
        
        size_t        B;
        
        double        delta;
        
        bool load( SEXP data1, SEXP data2, SEXP windows, SEXP Rdelta, SEXP RB ) throw()
        {
            //==================================================================
            // extract parameters from data1
            //==================================================================
            SEXP Rdim1 = getAttrib(data1, R_DimSymbol);
            nrow1      = INTEGER(Rdim1)[0];
            ncol1      = INTEGER(Rdim1)[1];
            Rprintf("\t[WINK] N1: nrow=%u, ncol=%u\n", unsigned(nrow1), unsigned(ncol1) );
            
            //==================================================================
            // extract parameters from data2
            //==================================================================
            SEXP Rdim2 = getAttrib(data2, R_DimSymbol);
            nrow2 = INTEGER(Rdim2)[0];
            ncol2 = INTEGER(Rdim2)[1];
            Rprintf("\t[WINK] N2: nrow=%u, ncol=%u\n", unsigned(nrow2), unsigned(ncol2) );
            
            
            if( nrow1 != nrow2 )
            {
                Rprintf("*** Error: different #trials\n");
                return false;
            }
            
            //==================================================================
            // extract parameters from windows
            //==================================================================
            SEXP RdimW = getAttrib(windows, R_DimSymbol);
            if( 2 != INTEGER(RdimW)[0] )
            {
                Rprintf("*** Error: windows must have 2 rows");
                return false;
            }
            
            num_windows = INTEGER(RdimW)[1];
            ptr_windows = REAL(windows);
            Rprintf("\t[WINK] number of windows=%u\n",unsigned(num_windows));
            if( num_windows <= 0 )
            {
                Rprintf("*** Error: invalid #windows\n");
                return false;
            }
            
            //==========================================================================
            // Get delta value
            //==========================================================================
            Rdelta = coerceVector(Rdelta,REALSXP);
            delta  = REAL(Rdelta)[0];
            Rprintf("\t[WINK] delta=%g\n",delta);
            
            //==========================================================================
            // Get B Count
            //==========================================================================
            RB = coerceVector(RB, INTSXP);
            B  = INTEGER(RB)[0];
            Rprintf("\t[WINK] B=%u\n",unsigned(B));
            if( B <= 0 )
            {
                Rprintf("*** Error: invalid B<=0\n");
                return false;
            }
            
            
            return true;
        }
        
    };
    
}


////////////////////////////////////////////////////////////////////////////////
//
// SERIAL CODE
//
////////////////////////////////////////////////////////////////////////////////
extern "C"
SEXP wink_ser( SEXP data1, SEXP data2, SEXP windows, SEXP Rdelta, SEXP RB) throw()
{
    
    parameters param;
    if( !param.load(data1,data2,windows,Rdelta,RB) )
        return R_NilValue;
    
    try 
    {
        //======================================================================
        // transform R data intro C++ objects
        //======================================================================
        wink::c_matrix     M1(param.nrow1,param.ncol1);
        wink::c_matrix     M2(param.nrow2,param.ncol2);
        
        M1.loadR( REAL(data1) );
        M2.loadR( REAL(data2) );
        
        //======================================================================
        // make C++ neuro_pair, init random generator
        //======================================================================
        wink::neuro_pair   NP(M1,M2,param.B);
        NP.g.seed( wink::neuro_pair::shared_seed + uint32_t(time(NULL)) );
       
        
        //======================================================================
        // create the return vector
        //======================================================================
        SEXP Rval;
        PROTECT(Rval = allocVector(REALSXP,param.num_windows) );
        double *ans = REAL(Rval);
        
        //======================================================================
        // outer loop on windows
        //======================================================================
        for( size_t i=0; i < param.num_windows; ++i )
        {
            //------------------------------------------------------------------
            // get the boundaries
            //------------------------------------------------------------------
            const double a = param.ptr_windows[0+2*i];
            const double b = param.ptr_windows[1+2*i];
            
            //------------------------------------------------------------------
            // call the integrated function
            //------------------------------------------------------------------
            const double pvalue = NP.pvalue_geq(a,b,param.delta);
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

extern "C"
SEXP wink_both_ser( SEXP data1, SEXP data2, SEXP windows, SEXP Rdelta, SEXP RB) throw()
{
    
    parameters param;
    if( !param.load(data1,data2,windows,Rdelta,RB) )
        return R_NilValue;
    
    try 
    {
        //======================================================================
        // transform R data intro C++ objects
        //======================================================================
        wink::c_matrix     M1(param.nrow1,param.ncol1);
        wink::c_matrix     M2(param.nrow2,param.ncol2);
        
        M1.loadR( REAL(data1) );
        M2.loadR( REAL(data2) );
        
        //======================================================================
        // make C++ neuro_pair, init random generator
        //======================================================================
        wink::neuro_pair   NP(M1,M2,param.B);
        NP.g.seed( wink::neuro_pair::shared_seed + uint32_t(time(NULL)) );
        
        
        //======================================================================
        // create the return matrix
        //======================================================================
        SEXP Rval;
        PROTECT(Rval = allocMatrix(REALSXP,2,param.num_windows) );
        double *ans = REAL(Rval);
        
        //======================================================================
        // outer loop on windows
        //======================================================================
        for( size_t i=0; i < param.num_windows; ++i )
        {
            const size_t i2 = i*2;
            const size_t i3 = i2+1;
            //------------------------------------------------------------------
            // get the boundaries
            //------------------------------------------------------------------
            const double a = param.ptr_windows[i2];
            const double b = param.ptr_windows[i3];
            
            //------------------------------------------------------------------
            // call the integrated function
            //------------------------------------------------------------------
            double &pvalue_geq = ans[ i2 ];
            double &pvalue_leq = ans[ i3 ];
            //Rprintf("[%10.6f,%10.6f] :  true_coinc=%6u : pvalue= %.8f\n",a,b,unsigned(NP.true_coinc),pvalue);
            NP.both_pvalues(pvalue_geq,pvalue_leq,a,b,param.delta);
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



////////////////////////////////////////////////////////////////////////////////
//
// PARALLEL CODE
//
////////////////////////////////////////////////////////////////////////////////
extern "C"
SEXP wink_par( SEXP data1, SEXP data2, SEXP windows, SEXP Rdelta, SEXP RB, SEXP Rnt) throw()
{
    
    
    parameters param;
    if( !param.load(data1,data2,windows,Rdelta,RB) )
        return R_NilValue;
    
    //==========================================================================
    // Get num_threads
    //==========================================================================
    Rnt = coerceVector(Rnt,INTSXP);
    size_t num_threads = INTEGER(Rnt)[0];
    Rprintf("\t#### num_threads=%u\n",unsigned(num_threads));
    if(num_threads<=0)
        num_threads = 1;
    
    
    try
    {
        //======================================================================
        // transform R data intro C++ objects
        //======================================================================
        wink::c_matrix     M1(param.nrow1,param.ncol1);
        wink::c_matrix     M2(param.nrow2,param.ncol2);
        
        M1.loadR( REAL(data1) );
        M2.loadR( REAL(data2) );
        
        //======================================================================
        // create the return vector
        //======================================================================
        SEXP Rval;
        PROTECT(Rval = allocVector(REALSXP,param.num_windows) );
        double *pvalues = REAL(Rval);
        
        //======================================================================
        // create the team of threads, starting ASAP
        //======================================================================
        wink::workers team(M1,
                           M2,
                           param.B,
                           num_threads,
                           param.num_windows,
                           param.ptr_windows,
                           pvalues,
                           param.delta,
                           wink::compute_pvalues_geq);
        
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

extern "C"
SEXP wink_both_par( SEXP data1, SEXP data2, SEXP windows, SEXP Rdelta, SEXP RB, SEXP Rnt) throw()
{
    
    
    parameters param;
    if( !param.load(data1,data2,windows,Rdelta,RB) )
        return R_NilValue;
    
    //==========================================================================
    // Get num_threads
    //==========================================================================
    Rnt = coerceVector(Rnt,INTSXP);
    size_t num_threads = INTEGER(Rnt)[0];
    Rprintf("\t#### num_threads=%u\n",unsigned(num_threads));
    if(num_threads<=0)
        num_threads = 1;
    
    
    try
    {
        //======================================================================
        // transform R data intro C++ objects
        //======================================================================
        wink::c_matrix     M1(param.nrow1,param.ncol1);
        wink::c_matrix     M2(param.nrow2,param.ncol2);
        
        M1.loadR( REAL(data1) );
        M2.loadR( REAL(data2) );
        
        //======================================================================
        // create the return vector
        //======================================================================
        SEXP Rval;
        PROTECT(Rval = allocMatrix(REALSXP,2,param.num_windows) );
        double *pvalues = REAL(Rval);
        
        //======================================================================
        // create the team of threads, starting ASAP
        //======================================================================
        wink::workers team(M1,
                           M2,
                           param.B,
                           num_threads,
                           param.num_windows,
                           param.ptr_windows,
                           pvalues,
                           param.delta,
                           wink::compute_pvalues_both);
        
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

