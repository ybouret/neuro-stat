#include "./lookup.hpp"
#include <cassert>

namespace wink
{
    
    bool lookup:: index_after( double a, const double *X, size_t &i )
    {
        assert(X!=NULL);
        const size_t Nx = size_t(X[0]);
        
        switch( Nx )
        {
            case 0:
                i=0;
                return false;
                
            case 1:
                if( X[1] >= a )
                {
                    i=1;
                    return true;
                }
                else
                {
                    i=0;
                    return false;
                }
                break;
                
            default:
                assert(Nx>=2);
                if( a > X[Nx] )
                {
                    //--------------------------------------------------------------
                    // trivial case
                    //--------------------------------------------------------------
                    i=0;
                    return false;
                }
                else 
                {
                    if( X[1] >= a )
                    {
                        //----------------------------------------------------------
                        // another trivial case
                        //----------------------------------------------------------
                        i = 1;
                        return true;
                    }
                    else 
                    {
                        //----------------------------------------------------------
                        // generic case: bisection
                        //----------------------------------------------------------
                        size_t ilo = 1;
                        size_t ihi = Nx;
                        while( ihi-ilo > 1 )
                        {
                            const size_t mid = (ilo+ihi) >> 1;
                            if( X[mid] >= a )
                                ihi = mid;
                            else 
                                ilo = mid;
                        }
                        i = ihi;
                        assert(i>0);
                        assert(a<=X[i]);
                        return true;
                    }
                    
                }
        }
    }
    
    
    bool lookup:: index_before( double b, const double *X, size_t &i )
    {
        
        assert(X!=NULL);
        const size_t Nx = size_t(X[0]);
        
        switch( Nx )
        {
            case 0:
                i=0;
                return false;
                
            case 1:
                if( X[1] <= b )
                {
                    i=1;
                    return true;
                }
                else
                {
                    i=0;
                    return false;
                }
                break;
                
            default:
                assert(Nx>=2);
                if( b >= X[Nx] )
                {
                    //--------------------------------------------------------------
                    // trivial case
                    //--------------------------------------------------------------
                    i = Nx;
                    return true;
                }
                else
                {
                    if( b < X[1] )
                    {
                        //----------------------------------------------------------
                        // another trivial case
                        //----------------------------------------------------------
                        i=0;
                        return false;
                    }
                    else 
                    {
                        //----------------------------------------------------------
                        // generic case: bisection
                        //----------------------------------------------------------
                        size_t ilo = 1;
                        size_t ihi = Nx;
                        while( ihi-ilo > 1 )
                        {
                            const size_t mid = (ilo+ihi) >> 1;
                            if( X[mid] < b )
                                ilo = mid;
                            else 
                                ihi = mid;
                        }
                        i = ilo;
                        assert(i>0);
                        assert(X[i]<=b);
                        return true;
                    }
                }
                return false;
        }
        
    }

}
