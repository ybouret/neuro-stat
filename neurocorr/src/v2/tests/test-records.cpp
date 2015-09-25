#include "../records.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/code/rand.hpp"


YOCTO_UNIT_TEST_IMPL(records)
{
    const size_t  num_neurones = 2 + alea_leq(50);
    const size_t  num_trials   = 3 + alea_leq(200);
    const size_t  num_trains   = num_neurones * num_trials;
    const size_t  max_spikes   = 100 + alea_leq(10000);
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

    Records records(1.0,neurodata,num_neurones);
    std::cerr << "#Neurons: " << records.neurones << std::endl;
    std::cerr << "#Trials:  " << records.trials  << std::endl;
    records.displayNeurones();
    
}
YOCTO_UNIT_TEST_DONE()
