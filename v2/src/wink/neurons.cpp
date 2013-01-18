#include "./neurons.hpp"
#include "../pyck/wtime.hpp"

#include <iostream>

namespace wink
{
    
    
    Mutex    neurons::access;
    uint32_t neurons::seed32;
    
    neurons:: ~neurons() throw()
    {
    }
    
    
    
    neurons::neurons() throw() :
    drawing(),
    ran()
    {
        PYCK_LOCK(access);
        const uint32_t s32 = IntegerHash32( WallTime::Seed() + seed32++ );
        //std::cerr << "s32=" << s32 << std::endl;
        ran.seed(s32);
    }
    
    
    size_t neurons:: coincidences( const neuron &N1, const neuron &N2, double delta ) const
    {
        size_t       count  = 0;
        for( const couple *c = head;c;c=c->next)
        {
            assert(c->first  < N1.trials);
            assert(c->second < N2.trials);
            const record &r1 = N1[ c->first  ];
            const record &r2 = N2[ c->second ];
            count += r1.coincidences_with(r2, delta);
        }
        return count;
    }
    
    size_t neurons:: true_coincidences(  neuron &N1, neuron &N2, const double a, const double b, const double delta)
    {
        const size_t trials = N1.trials;
        if( N2.trials != trials )
            throw Exception("#trials mismatch for true_coincidences");
        
        //! preparing the windows
        N1.prepare_windows(a, b);
        N2.prepare_windows(a, b);
        
        //! setting identity drawing
        identity(trials);
        
        //! evaluate coincidences
        return coincidences(N1, N2, delta);
    }
    
    void neurons:: bootstrap( C_Array<size_t> &Bcoinc, bootstrap_method Bkind, const neuron &N1, const neuron &N2, double delta )
    {
        const size_t nb = Bcoinc.size;
        const size_t n1 = N1.trials;
        const size_t n2 = N2.trials;
        switch( Bkind )
        {
            case bootstrap_perm:
                if(n1!=n2) throw Exception("#trials mismatch for bootstrap/permutations");
                for( size_t i=0; i < nb; ++i )
                {
                    permutation(n1, ran);
                    Bcoinc[i] = coincidences(N1, N2, delta);
                }
                break;
                
            case bootstrap_repl:
                if(n1!=n2) throw Exception("#trials mismatch for bootstrap/permutations");
                for( size_t i=0; i < nb; ++i )
                {
                    replacement(n1, ran);
                    Bcoinc[i] = coincidences(N1, N2, delta);
                }
                break;
        }
    }
    
    void neurons:: compute_pvalues_T(double                &alpha_minus,
                                     double                &alpha_plus,
                                     const C_Array<size_t> &Bcoinc,
                                     const size_t           Tcoinc) const throw()
    {
        const size_t nb          = Bcoinc.size;
        size_t       count_plus  = 1;
        size_t       count_minus = 1;
        for( size_t i=0; i < nb; ++i )
        {
            const size_t bvalue = Bcoinc[i];
            if( bvalue >= Tcoinc)
            {
                ++count_plus;
            }
            else
            {
                if(bvalue<=Tcoinc)
                    ++count_minus;
            }
        }
        
        const size_t den = 1+nb;
        alpha_plus       = double(count_plus)/den;
        alpha_minus      = double(count_minus)/den;
    }
    
    
}
