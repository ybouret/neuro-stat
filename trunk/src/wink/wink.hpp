//! \file
/**
 \brief Wink Is a NeuroStats Kernel.
 */
#ifndef WINK_INCLUDED
#define WINK_INCLUDED 1

#include "wink-neuro-trials.hpp"
#include "wink-permutation.hpp"

namespace wink
{
        
    //! total coincidences on a prepared window.
    /**
     The windows must have been prepared before: 
     N1.prepare_windows(a,b);
     N2.prepare_windows(a,b);
     */
    size_t total_coincidences(const neuro_trials &N1, 
                              const neuro_trials &N2, 
                              const double       delta,
                              const permutation &perm ) throw();
    
    
    //! true coincidences on a prepared window
    /**
     call perm.identity() and total_coincidences(N1,N2,perm,delta);
     */
    size_t true_coincidences(const neuro_trials &N1, 
                             const neuro_trials &N2, 
                             const double       delta,
                             permutation       &perm) throw();
    
    //! bootstrap coincidences on a prepared window
    /**
     call perm.rebuild(g)
     and total_coincidences(N1,N2,perm,delta);
     */
    size_t bootstrap_coincidences(const neuro_trials &N1, 
                                  const neuro_trials &N2, 
                                  const double       delta,
                                  permutation       &perm,
                                  urand32           &g) throw();
    
    //! create a bootstrap sample
    /**
     \param Bcoinc coincidences
     \param Bcount Bcoinc[0..Bcount-1]
     */
    void permutation_bootstrap(size_t             *Bcoinc, 
                               const size_t        Bcount, 
                               const neuro_trials &N1, 
                               const neuro_trials &N2, 
                               const double       delta,
                               permutation       &perm,
                               urand32           &g) throw();
    
    //! find the pvalue from a sample and a true coincidence
    double permutation_pvalue( const size_t true_coinc, const size_t *Bcoinc, const size_t Bcount ) throw();
    
}

#endif
