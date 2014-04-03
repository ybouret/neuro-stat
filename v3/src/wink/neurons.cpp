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
    ran(),
    M()
    {
        PYCK_LOCK(access);
        const uint32_t s32 = IntegerHash32( WallTime::Seed() + seed32++ );
        ran.seed(s32);
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
        //std::cerr << "CorrMat=" << M << std::endl;
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
        assert(this->size>0);
        const count_t n      = count_t(this->size); //!< #couples
        
        //----------------------------------------------------------------------
        // build the index arrays
        //----------------------------------------------------------------------
        C_Array<size_t> v(n<<1);
        size_t *c1 = &v[0];
        size_t *c2 = c1+n;
        
        unsigned idx = 0;
        for( const couple *c = head; c; c=c->next, ++idx )
        {
            c1[idx] = c->first;  assert(c->first<M.rows);
            c2[idx] = c->second; assert(c->second<M.cols);
        }
        
        //----------------------------------------------------------------------
        // compute H from the correlation matrix
        //----------------------------------------------------------------------
        count_t direct  = 0;
        count_t crossed = 0;
        for(count_t i=0; i < n; ++i )
        {
            const count_t c1i = c1[i];
            direct += M[c1i][c2[i]];
            for(count_t j=0;j<n;++j)
            {
                if(i!=j)
                {
                    crossed += M[c1i][c2[j]];
                }
            }
        }
        
        return 2 * ( (n-1) * direct - crossed);
        
     
    }
    
    count_t neurons:: true_coincidences(  statistic_value S, neuron &N1, neuron &N2, const double a, const double b, const double delta)
    {
        assert(N1.trials==N2.trials);
        
        //----------------------------------------------------------------------
        //
        // initialize all windows and
        // compute the coincidence matrix
        //
        //----------------------------------------------------------------------
        initialize_correlations(N1,N2,a,b,delta);
        
        //----------------------------------------------------------------------
        //
        // set this drawing to identity
        //
        //----------------------------------------------------------------------
        identity(N1.trials);
        
        //----------------------------------------------------------------------
        //
        // return the true coincidences
        //
        //----------------------------------------------------------------------
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
                
            case mix_shuf:
                gen = & drawing:: trial_shuffling;
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
