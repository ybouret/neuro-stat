#include "wink.hpp"
#include <cassert>
#include <iostream>

namespace wink 
{
    void neuro_trials:: display() const
    {
        std::cerr << "\t#trials=" << num << std::endl;
        std::cerr << "\t#top   =" << top << std::endl;
        for( size_t i=0; i < num; ++i )
        {
            std::cerr <<  rec[i].X[0] << "/";
        }
        std::cerr << std::endl;
    }
    
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
        assert(data!=NULL);
        rec = new record[num];
        
        //======================================================================
        // link resources
        //======================================================================
        for( size_t i=0; i < num; ++i )
        {
            record &r = rec[i];
            r.X = data + (i*nc);
            if( r.X[0] > top )
            {
                delete []rec;
                throw "invalid record size";
            }
        }
        
    }
    
    
    void neuro_trials:: prepare_windows( double a, double b ) throw()
    {
        for( size_t i=0; i<num; ++i )
        {
            record &r = rec[i];
            r.W.initialize(a,b, r.X);
        }
    }
    
    
    neuro_trials::record::record() throw() : X(0), W()
    {
    }
    
    neuro_trials::record::~record() throw() 
    {
    }
    
        
}
