#include "neurodata.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/code/rand.hpp"

//using namespace math;

YOCTO_UNIT_TEST_IMPL(neurodata)
{

    for(size_t iter=0;iter<100;++iter)
    {
        const size_t nr = 1 + alea_leq(20);
        const size_t nt = 1 + alea_leq(100);
        const size_t ni = 1 + alea_leq(1000);


        NeuroData ND(nr,nt,ni);
        std::cerr << std::endl;
        std::cerr << "\t(*) #neurons=" << ND.neurons << std::endl;
        std::cerr << "\t(*) #trials =" << ND.trials  << std::endl;
        std::cerr << "\t(*) #trains =" << ND.trains  << std::endl;
        std::cerr << "\t(*) #maxinp =" << ND.max_spikes_per_train  << std::endl;

        for(size_t i=0;i<ND.trains;++i)
        {
            RArray<double> &input = ND.get_raw_input(i);
            const size_t ns = alea_leq(ni);
            input[0] = ns;
            assert(ns<=input.size()+1);
            for(size_t i=1;i<=ns;++i)
            {
                input[i] = alea<double>();
            }
        }

        ND.setup();
        for(size_t i=0;i<ND.neurons;++i)
        {
            const Neuron &n = ND.neuron[i];
            std::cerr << "Neuron[" << i << "] : #trials=" << n.trials << std::endl;
        }

        for(size_t i=0;i<ND.trials;++i)
        {
            const Trial &t = ND.trial[i];
            std::cerr << "Trial[" << i << "] : #neurons=" << t.neurons << std::endl;
        }
    }


}
YOCTO_UNIT_TEST_DONE()