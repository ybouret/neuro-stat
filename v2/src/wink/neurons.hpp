#ifndef WINK_NEURONS_INCLUDED
#define WINK_NEURONS_INCLUDED 1

#include "./neuron.hpp"
#include "./drawing.hpp"

namespace wink
{
    enum statistic_value
    {
        statistic_T,
        statistic_H
    };
    
    enum bootstrap_method
    {
        bootstrap_perm,
        bootstrap_repl
    };
    
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
        count_t true_coincidences(  statistic_value S, neuron &N1, neuron &N2, const double a, const double b, const double delta);
        
        
        //! bootstrap
        /**
         \param N1 a prepared neuron on window [a,b]
         \param N2 a prepared neuron on window [a,b]
         */
        void bootstrap( statistic_value S, C_Array<count_t> &Bcoinc, bootstrap_method Bkind, const neuron &N1, const neuron &N2, double delta );
        
        //! compute p_values for statistics T
        void compute_pvalues(double                 &alpha_minus,
                             double                 &alpha_plus,
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
