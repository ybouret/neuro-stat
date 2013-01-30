/**
 \file
 \brief operations on a pair of neurons.
 */
#ifndef WINK_NEURONS_INCLUDED
#define WINK_NEURONS_INCLUDED 1

#include "./neuron.hpp"
#include "./drawing.hpp"
#include "../pyck/c_matrix.hpp"

namespace wink
{
    //! statistic choice
    enum statistic_value
    {
        statistic_T,
        statistic_H
    };
    
    //! mix choice
    enum mix_method
    {
        mix_perm,
        mix_repl,
        mix_boot
    };
    
    //! inherits from drawing.
    class neurons : public drawing
    {
    public:
        explicit neurons() throw();
        virtual ~neurons() throw();
        
        DefaultUniformGenerator ran; //!< own random number generator
        C_Matrix<count_t>       M;   //!< matrix of precomputed coincidences
        
        //! sum coincidences for N1 and N2 for CURRENT drawing
        /**
         \param N1 a prepared neuron on window [a,b]
         \param N2 a prepared neuron on window [a,b]
         \param delta time lag
         */
        count_t coincidences_T( const neuron &N1, const neuron &N2, double delta ) const;
        
        //! sum coincidences for N1 and N2 for CURRENT drawing
        /**
         \param N1 a prepared neuron on window [a,b]
         \param N2 a prepared neuron on window [a,b]
         \param delta time lag
         */
        count_t coincidences_H( const neuron &N1, const neuron &N2, double delta ) const;
        
        //! initialize neurons and drawing=id, then call coincidences
        /**
         \param S the statistic value (T,H,...)
         \param N1 first  neuron
         \param N2 second neuron
         */
        count_t true_coincidences(  statistic_value S, neuron &N1, neuron &N2, const double a, const double b, const double delta);
        
        
        //! mix for a given statistic
        /**
         \param N1 a prepared neuron on window [a,b]
         \param N2 a prepared neuron on window [a,b]
         */
        void mix( statistic_value S, C_Array<count_t> &Bcoinc, mix_method Bkind, const neuron &N1, const neuron &N2, double delta );
        
        //! compute p_values for a given statistic
        /**
         \param alpha_minus: 1/(B+1) (1+count(Bcounc[i]<=Tcoinc))
         \param alpha_plus:  1/(B+1) (1+count(Bcounc[i]>=Tcoinc))
         */
        void compute_pvalues(double                 &alpha_minus,
                             double                 &alpha_plus,
                             const C_Array<count_t> &Bcoinc,
                             const count_t           Tcoinc) const throw();
        
        //! compute count+ and count- for a given statistic
        void compute_counts(double                 &count_minus,
                            double                 &count_plus,
                            const C_Array<count_t> &Bcoinc,
                            const count_t           Tcoinc) const throw();
        
        
        //! prepare the neurons for window [a,b] and fill the diagonal part of the count matrix.
        /**
         \param N1 first  neuron
         \param N2 second neuron
         */
        void initialize_correlations( neuron &N1, neuron &N2, const double a, const double b, const double delta);
        
        
        //! get T coincidences for the CURRENT drawing, using the count matrix.
        count_t coincidences_T() const throw();
        
        //! get H coincidences for the CURRENT drawing, using the count matrix.
        count_t coincidences_H() const throw();
        
        //! initialize neurons,correlations and set drawing=id, then call coincidences
        /**
         \param S the statistic value (T,H,...)
         \param N1 first  neuron
         \param N2 second neuron
         */
        count_t self_coincidences(  statistic_value S, neuron &N1, neuron &N2, const double a, const double b, const double delta);
        
        //! eval for a given statistic
        /**
         must be called after a self_coincidences, so that the matrix is built
         */
        void eval_coincidences( statistic_value S, C_Array<count_t> &coinc, mix_method kind );
        
        
        static Mutex    access; //!< shared access
        static uint32_t seed32; //!< shared seed, for different random seeds
        
    private:
        neurons(const neurons &);
        neurons&operator=(const neurons &);
    };
    
}

#endif
