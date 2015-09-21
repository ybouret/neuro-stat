#include "../neuron.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/code/rand.hpp"

YOCTO_UNIT_TEST_IMPL(neuron)
{

    for(size_t iter=0;iter<100;++iter)
    {
        const size_t  num_neurons = 1+alea_leq(10);
        const size_t  num_trials  = 1+alea_leq(100);
        const size_t  nr = num_neurons*num_trials;
        const size_t  ns = alea_lt(100);
        const size_t  nc = 1 + ns;

        std::cerr << "-- num_neurons=" << num_neurons << std::endl;
        std::cerr << "-- num_trials =" << num_trials  << std::endl;
        std::cerr << "-- creating neuro data with " << nr << " rows" << std::endl;
        std::cerr << "-- max spikes=" << ns << std::endl;
        std::cerr << "-- columns   =" << nc << std::endl;
        CMatrix<Real> neurodata(nr,nc);
        for(size_t i=0;i<nr;++i)
        {
            const size_t nj = alea_leq(ns);
            //std::cerr << "\ttrain #" << i << ": #spikes=" << nj << std::endl;
            neurodata[i][0] = nj;
            for(size_t j=1;j<=nj;++j)
            {
                neurodata[i][j] = alea<double>();
            }
            if(nj>1)
            {
                LArray<double> tr(&neurodata[i][1],nj);
                RQSort(tr);
            }
        }

        Neurons neurons(1000.0,neurodata,num_neurons);
        std::cerr << "neurons.size=" << neurons.size << std::endl;
        for(size_t i=0;i<neurons.size;++i)
        {
            const Neuron &nn = neurons[i];
            std::cerr << "neuron #" << i << ": #trials=" << nn.trials << std::endl;
            for(size_t j=0;j<nn.trials;++j)
            {
                const Train &tr = nn[j];
                std::cerr << "/" << tr.size();
            }
            std::cerr << "." << std::endl;
        }
    }

}
YOCTO_UNIT_TEST_DONE()

