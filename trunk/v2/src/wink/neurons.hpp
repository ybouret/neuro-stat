#ifndef WINK_NEURONS_INCLUDED
#define WINK_NEURONS_INCLUDED 1

#include "./neuron.hpp"
#include "./drawing.hpp"

namespace wink
{
    
    class neurons : public drawing
    {
    public:
        explicit neurons() throw();
        virtual ~neurons() throw();
        
        DefaultUniformGenerator ran; //!< random number generator
        
        //! sum coincidences for N1 and N2 for CURRENT drawing
        /**
         \param N1 a prepared neuron on window [a,b]
         \param N2 a prepared neuron on window [a,b]
         \param delta time lag
         */
        size_t total_coincidences( const neuron &N1, const neuron &N2, double delta ) const;
        
        
        static Mutex    access; //!< shared access
        static uint32_t seed32; //!< shared seed, for different random seeds
        
        
    private:
        neurons(const neurons &);
        neurons&operator=(const neurons &);
    };
    
}

#endif
