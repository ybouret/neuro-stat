//! \file
/**
 \brief to work on a pair of neurons
 */

#ifndef WINK_NEURO_PAIR_INCLUDED
#define WINK_NEURO_PAIR_INCLUDED


#include "wink-neuro-trials.hpp"
#include "wink-permutation.hpp"
#include "wink-c-matrix.hpp"

namespace wink 
{
    
    class neuro_pair
    {
    public:
        virtual ~neuro_pair() throw();
        
        //! initialize the neuro_trials pair
        /**
         \param ntrial the common #trials
         \param data1  ntrials * ncol1 
         \param ncol1  max columns for data1
         \param data2  ntrials * ncol2
         \param B      #bootstraps
         */
        neuro_pair(size_t        ntrials, 
                   const double *data1, 
                   size_t        ncol1, 
                   const double *data2, 
                   size_t        ncol2,
                   size_t        B);
        
        //! initialize the neuro_trials from the matrices
        /**
         \param M1 data for first neuron
         \param M2 data for second neuron
         \param B  #bootstraps
         \remark M1 and M2 MUST have the same #rows
         */
        neuro_pair( const c_matrix &M1, const c_matrix &M2, size_t B );
        
        neuro_trials  N1;         //!< first neuron
        neuro_trials  N2;         //!< second neuron
        permutation   perm;       //!< permutation
        permutation   boot;       //!< bootstrap data
        size_t       *Bcoinc;     //!< store bootstrap coincidences
        const size_t &Bcount;     //!< number of bootstraps
        size_t        true_coinc; //!< last true_coinc
        rand32_kiss   g;          //!< own generator, to be seeded

        //! evaluate the pvalue on [a,b] with lag delta
        double pvalue( double a, double b, double delta ) throw();
        
        //! 
        
        
    private:
        neuro_pair( const neuro_pair & );
        neuro_pair&operator=( const neuro_pair & );
    };
    
    
}

#endif
