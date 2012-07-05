#include "wink.hpp"
#include "wink-utils.hpp"

namespace wink
{
    
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
                                  urand32           &g) throw()
    {
        perm.generate(g);
        return total_coincidences(N1, N2, delta, perm);
    }
    
    
    void permutation_bootstrap(size_t             *Bcoinc, 
                               const size_t        Bcount, 
                               const neuro_trials &N1, 
                               const neuro_trials &N2, 
                               const double       delta,
                               permutation       &perm,
                               urand32           &g) throw()
    {
        assert(!(Bcoinc==NULL&&Bcount>0));
        for( size_t i=0; i < Bcount; ++i )
        {
            Bcoinc[i] = bootstrap_coincidences(N1,N2,delta,perm,g);
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
