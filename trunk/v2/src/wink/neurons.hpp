/**
 \file
 \brief operations on a pair of neurons.
 */
#ifndef WINK_NEURONS_INCLUDED
#define WINK_NEURONS_INCLUDED 1

#include "./neuron.hpp"
#include "./drawing.hpp"

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
        
        static Mutex    access; //!< shared access
        static uint32_t seed32; //!< shared seed, for different random seeds
        
    private:
        neurons(const neurons &);
        neurons&operator=(const neurons &);
    };
    
}

#endif
