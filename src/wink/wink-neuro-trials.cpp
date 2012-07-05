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
    
    size_t total_coincidences(const neuro_trials &N1, 
                              const neuro_trials &N2, 
                              const double       delta,
                              const permutation &perm ) throw()
    {
        assert(N1.num==N2.num);
        assert(N1.num==perm.size);
        size_t        count = 0;
        const size_t  ntrials=N1.num;
        for( size_t i=0; i < ntrials; ++i )
        {
            const size_t j = perm.indx[i];
            assert(j<ntrials);
            const neuro_trials::record &r1 = N1.rec[i];
            const neuro_trials::record &r2 = N2.rec[j];
            count += coincidences(r1.X, r1.W, r2.X, r2.W, delta);
        }
        return count;
    }
    
    size_t true_coincidences(const neuro_trials &N1, 
                             const neuro_trials &N2, 
                             const double       delta,
                             permutation       &perm) throw()
    {
        perm.identity();
        return total_coincidences(N1,N2,delta,perm);
    }
    
    size_t bootstrap_coincidences(const neuro_trials &N1, 
                                  const neuro_trials &N2, 
                                  const double       delta,
                                  permutation       &perm,
                                  double            (*uniform_generator)()) throw()
    {
        //perm.rebuild(uniform_generator);
        return total_coincidences(N1, N2, delta, perm);
    }
    
    
    void permutation_bootstrap(size_t             *Bcoinc, 
                               const size_t        Bcount, 
                               const neuro_trials &N1, 
                               const neuro_trials &N2, 
                               const double       delta,
                               permutation       &perm,
                               double            (*uniform_generator)()) throw()
    {
        assert(!(Bcoinc==NULL&&Bcount>0));
        for( size_t i=0; i < Bcount; ++i )
        {
            Bcoinc[i] = bootstrap_coincidences(N1,N2,delta,perm,uniform_generator);
        }
        sort_indices(Bcoinc,Bcount);
    }
    
    double permutation_pvalue( const size_t true_coinc, const size_t *Bcoinc, const size_t Bcount ) throw()
    {
        assert(!(Bcoinc==NULL&&Bcount>0));
        size_t count = 0;
        for( size_t i=0; i < Bcount; ++i )
        {
            if(Bcoinc[i]>=true_coinc) ++count;
        }
        return double(count)/Bcount;
    }
    
}
