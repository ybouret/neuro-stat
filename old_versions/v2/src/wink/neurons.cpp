#include "./neurons.hpp"
#include "../pyck/wtime.hpp"

#include <iostream>

namespace wink
{
    
    
    Mutex    neurons::access;
    uint32_t neurons::seed32;
    
    neurons:: ~neurons() throw()
    {
        //std::cerr << "[Destructing Neurons @" << (void*)this << " ]" << std::endl;
    }
    
    
    
    neurons::neurons() throw() :
    drawing(),
    ran(),
    M()
    {
        PYCK_LOCK(access);
        const uint32_t s32 = IntegerHash32( WallTime::Seed() + seed32++ );
        ran.seed(s32);
        //std::cerr << "[Initializing Neurons @" << (void*)this << ", With Seed=" << s32 << "]" << std::endl;
    }
    
    
    count_t neurons:: coincidences_T( const neuron &N1, const neuron &N2, double delta ) const
    {
        count_t count  = 0;
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
    
    count_t neurons:: coincidences_H( const neuron &N1, const neuron &N2, double delta ) const
    {
        const size_t  ntrials = N1.trials; assert(ntrials>0);
        assert(N2.trials==N2.trials);
        assert(ntrials==size);
        count_t        count  = 0;
        const count_t  n1     = count_t(ntrials) - 1;
        for( const couple *I = head;I;I=I->next)
        {
            assert(I->first  < N1.trials);
            assert(I->second < N2.trials);
            
            const size_t  i      = I->first;
            const size_t  sig_i  = I->second;
            const record &X_i    = N1[i];
            const count_t diago  = X_i.coincidences_with(N2[sig_i], delta);
            count += n1 * diago;
            for( const couple *J=head;J;J=J->next)
            {
                const size_t j = J->first;
                if( j != i )
                {
                    const size_t  sig_j = J->second;
                    const count_t extra = X_i.coincidences_with( N2[sig_j], delta);
                    count -= extra;
                }
            }
        }
        return count;
    }
    
    count_t neurons:: true_coincidences(  statistic_value S, neuron &N1, neuron &N2, const double a, const double b, const double delta)
    {
        const size_t trials = N1.trials;
        if( N2.trials != trials )
            throw Exception("#trials mismatch for true_coincidences_T");
        
        //! preparing the windows
        N1.prepare_windows(a, b);
        N2.prepare_windows(a, b);
        
        //! setting identity drawing
        identity(trials);
        
        //! evaluate coincidences
        switch( S )
        {
            case statistic_T:
                return coincidences_T(N1, N2, delta);
                
            case statistic_H:
                return coincidences_H(N1, N2, delta);
        }
        throw Exception("unknown statistic type");
    }
    
    
    
    
    void neurons:: mix( statistic_value S, C_Array<count_t> &Bcoinc, mix_method Bkind, const neuron &N1, const neuron &N2, double delta )
    {
        const size_t nb = Bcoinc.size;
        const size_t n1 = N1.trials;
        const size_t n2 = N2.trials;
        if(n1!=n2) throw Exception("#trials mismatch for bootstrap");
        
        // fetch the coincidence method
        count_t (neurons:: *coinc)( const neuron &N1, const neuron &N2, double delta ) const = 0;
        switch( S )
        {
            case statistic_T:
                coinc = & neurons:: coincidences_T;
                break;
                
            case statistic_H:
                coinc = & neurons:: coincidences_H;
                break;
                
        }
        
        // fetch the generator method
        void (drawing:: *gen)( size_t, UniformGenerator &) = 0;
        switch( Bkind )
        {
            case mix_perm:
                gen = & drawing:: permutation;
                break;
                
            case mix_repl:
                gen = & drawing:: replacement;
                break;
                
            case mix_boot:
                gen = & drawing:: bootstrap2;
                break;
        }
        
        for( size_t i=0; i < nb; ++i )
        {
            // generate couples
            ( *this.*gen)(n1,ran);
            
            // compute coincidences
            Bcoinc[i] = (*this.*coinc)(N1,N2,delta);
        }
        
    }
    
    
    void neurons:: compute_pvalues(double                 &alpha_minus,
                                   double                 &alpha_plus,
                                   const C_Array<count_t> &Bcoinc,
                                   const count_t           Tcoinc) const throw()
    {
        const size_t nb          = Bcoinc.size;
        size_t       count_plus  = 1;
        size_t       count_minus = 1;
        for( size_t i=0; i < nb; ++i )
        {
            const count_t bvalue = Bcoinc[i];
            if( bvalue >= Tcoinc)
                ++count_plus;
            
            if(bvalue<=Tcoinc)
                ++count_minus;
        }
        
        const size_t den = 1+nb;
        alpha_plus       = double(count_plus)/den;
        alpha_minus      = double(count_minus)/den;
    }
    
    void neurons:: compute_counts(double                 &count_minus,
                                  double                 &count_plus,
                                  const C_Array<count_t> &Bcoinc,
                                  const count_t           Tcoinc) const throw()
    {
        const size_t nb = Bcoinc.size;
        count_minus     = 0;
        count_plus      = 0;
        for( size_t i=0; i < nb; ++i )
        {
            const count_t bvalue = Bcoinc[i];
            
            if( bvalue >= Tcoinc)
                ++count_plus;
            
            if(bvalue<=Tcoinc)
                ++count_minus;
        }
    }
    
    void neurons:: initialize_correlations( neuron &N1, neuron &N2, const double a, const double b, const double delta)
    {
        const size_t n1 = N1.trials;
        const size_t n2 = N2.trials;
        if(n1!=n2) throw Exception("#trials mismatch for initialize_correlation");
        const size_t n = n1;
        
        //! prepare the matrix
        M.make(n,n);
        
        //! preparing the windows
        N1.prepare_windows(a, b);
        N2.prepare_windows(a, b);
        
        //! building the correlation matrix
        for( size_t i=0; i < n; ++i )
        {
            for(size_t j=0; j < n; ++j )
            {
                M[i][j] = N1[i].coincidences_with(N2[j], delta);
            }
        }
    }
    
    
    count_t neurons:: coincidences_T() const throw()
    {
        assert(M.rows==M.cols);
        count_t ans = 0;
        for( const couple *c = head;c;c=c->next)
        {
            const size_t i = c->first;  assert(i<M.rows);
            const size_t j = c->second; assert(j<M.cols);
            ans += M[i][j];
        }
        return ans;
    }
    
    count_t neurons:: coincidences_H() const throw()
    {
        count_t       count  = 0;
        const count_t n      = count_t(this->size); //!< #couples
        const count_t n1     = n-1;
        for( const couple *I = head;I;I=I->next)
        {
            
            const size_t   i      = I->first;   assert(i<M.rows);
            const size_t   sig_i  = I->second;  assert(sig_i<M.cols);
            count += n1 * M[i][sig_i];
            
            for( const couple *J=head;J;J=J->next)
            {
                const size_t j = J->first;
                if( j != i )
                {
                    const size_t sig_j = J->second; assert( sig_j < M.cols);
                    count -= M[i][sig_j];
                    
                }
            }
        }
        return count;
        
    }
    
    count_t neurons:: self_coincidences(  statistic_value S, neuron &N1, neuron &N2, const double a, const double b, const double delta)
    {
        initialize_correlations(N1,N2,a,b,delta);
        assert(N1.trials==N2.trials);
        identity(N1.trials);
        count_t ans = 0;
        switch( S )
        {
            case statistic_T:
                ans = coincidences_T();
                break;
                
            case statistic_H:
                ans = coincidences_H();
                break;
        }
        return ans;
    }
    
    void neurons:: eval_coincidences( statistic_value S, C_Array<count_t> &coinc, mix_method kind )
    {
        assert(M.rows==M.cols);
        const size_t n  = M.rows;
        const size_t nb = coinc.size;
        
        
        // fetch the coincidence method
        count_t (neurons:: *proc)(void) const = 0;
        switch( S )
        {
            case statistic_T:
                proc = & neurons:: coincidences_T;
                break;
                
            case statistic_H:
                proc = & neurons:: coincidences_H;
                break;
                
        }
        
        // fetch the generator method
        void (drawing:: *gen)( size_t, UniformGenerator &) = 0;
        switch( kind )
        {
            case mix_perm:
                gen = & drawing:: permutation;
                break;
                
            case mix_repl:
                gen = & drawing:: replacement;
                break;
                
            case mix_boot:
                gen = & drawing:: bootstrap2;
                break;
        }
        
        for( size_t i=0; i < nb; ++i )
        {
            // generate couples
            ( *this.*gen)(n,ran);
            
            // compute coincidences
            coinc[i] = (*this.*proc)();
        }

    }

    
    
}
