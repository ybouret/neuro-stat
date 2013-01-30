#include "../pyck/R++.hpp"
#include "../wink/neurons.hpp"

using namespace wink;

extern "C"
SEXP wink_version()
{
    Rprintf("Compiled on %s\n", __DATE__);
    return R_NilValue;
}


static
neurons & shared_neurons()
{
    static neurons n;
    return n;
}

extern "C"
SEXP wink_perm( SEXP Rn )
{
    try
    {
        const size_t n = R2Scalar<int>(Rn);
        if( n <= 0 )
        {
            throw Exception("required length is negative!");
        }
        
        RVector<int>         ans(n);
        for(size_t i=0; i<n;++i)
        {
            ans[i] = int(i);
        }
        shared_neurons().ran.shuffle( &ans[0], n);        
        return *ans;
    }
    catch( const Exception &e )
    {
        Rprintf("*** wink_perm: %s\n", e.what());
    }
    catch(...)
    {
        Rprintf("*** unhanled exception in wink_perm\n");
    }
    return R_NilValue;
}

