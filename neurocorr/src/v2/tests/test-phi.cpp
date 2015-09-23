#include "../phi.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/code/rand.hpp"
#include "yocto/sys/wtime.hpp"

YOCTO_UNIT_TEST_IMPL(phi)
{
    wtime chrono;
    chrono.start();
    uint64_t mark = 0;
    
    {
        const size_t  num_neurons = 2 + alea_leq(50);
        const size_t  num_trials  = 3 + alea_leq(200);
        const size_t  num_trains  = num_neurons * num_trials;
        const size_t  max_spikes  = 100 + alea_leq(10000);
        CMatrix<Real> neurodata(num_trains,1+max_spikes);
        for(size_t i=0;i<num_trains;++i)
        {
            const size_t ns = alea_leq(max_spikes);
            neurodata[i][0] = ns;
            size_t curr = 0;
            for(size_t j=1;j<=ns;++j)
            {
                curr += 1 + alea_leq(10);
                neurodata[i][j] = curr;
            }
        }

        Neurons neurons(1,neurodata,num_neurons);
        std::cerr << "#neurons=" << neurons.size << std::endl;
        for(size_t i=0;i<neurons.size;++i)
        {
            const Neuron &nn = neurons[i];
            std::cerr << "neuron #" << i << std::endl;
            nn.displayInfo();
        }

        std::cerr << "Allocating Memory for Phi..." << std::endl;
        const size_t extra = 2;
        PhiPerNeurons phi(extra,neurons);
        std::cerr << "Ready for " << phi.trains() << " Phi functions" << std::endl;
        
    }

}
YOCTO_UNIT_TEST_DONE()
