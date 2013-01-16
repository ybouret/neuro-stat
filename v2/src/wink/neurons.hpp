#ifndef WINK_NEURONS_INCLUDED
#define WINK_NEURONS_INCLUDED 1

#include "./one-neuron.hpp"
#include "./drawing.hpp"

namespace wink
{
    
    class neurons
    {
    public:
        explicit neurons() throw();
        virtual ~neurons() throw();
        
        DefaultUniformGenerator ran; //!< random number generator
        drawing                 drw; //!< drawing generator
        
        static Mutex    access; //!< shared access
        static uint32_t seed32; //!< shared seed, for different random seeds
                
    private:
        neurons(const neurons &);
        neurons&operator=(const neurons &);
    };
    
}

#endif
