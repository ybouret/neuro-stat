#include "wink.hpp"
#include <cassert>

namespace wink 
{
    
    neuro_trials:: ~neuro_trials() throw()
    {
        delete []rec;
    }
    
    
    neuro_trials:: neuro_trials( const double *data, const size_t nr, const size_t nc ) :
    num(nr),
    top(nc-1),
    rec(0)
    {
        //======================================================================
        // allocate resources
        //======================================================================
        assert(nr>0);
        assert(nc>0);
        rec = new record[num];
        
        //======================================================================
        // link resources
        //======================================================================
        for( size_t i=0; i < num; ++i )
        {
            rec[i].X = data + (i*nc);
        }
    }
    
    
    void neuro_trials:: prepare_windows( double a, double b ) throw()
    {
        for( size_t i=0; i < num; ++i )
        {
            record &r = rec[num];
            r.W.initialize(a,b, r.X);
        }
    }

}