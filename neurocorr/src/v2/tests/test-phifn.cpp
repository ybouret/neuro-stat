#include "../box.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/code/rand.hpp"
#include "yocto/sys/wtime.hpp"

YOCTO_UNIT_TEST_IMPL(phifn)
{

    wtime chrono;
    chrono.start();
    uint64_t mark = 0;
    Crew para;

    const size_t  num_neurons = 2 + alea_leq(50);
    const size_t  num_trials  = 3 + alea_leq(200);
    const size_t  num_trains  = num_neurons * num_trials;
    const size_t  max_spikes  = 100 + alea_leq(10000);
    CMatrix<Real> neurodata(num_trains,1+max_spikes);

    std::cerr << "Creating Data for " << num_trains << " trains" << std::endl;
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

    Records records(1.0,neurodata,num_neurons);
    records.displayNeurons();

    //! allocating memory
    std::cerr << "Allocating Memory for Phi" << std::endl;
    const size_t extra = 2;
    PHI phi(extra,records);


    std::cerr << "Computing, Sequential" << std::endl;
    mark = chrono.ticks();
    phi.compute(1,NULL);
    const double seqTime = chrono( chrono.ticks()-mark );
    std::cerr << "seqTime=" << seqTime << std::endl;

    std::cerr << "Computing, Parallel" << std::endl;
    mark = chrono.ticks();
    phi.compute(1,&para);
    const double parTime = chrono( chrono.ticks()-mark );
    std::cerr << "parTime=" << parTime << std::endl;
    std::cerr << "SpeedUp=" << seqTime/parTime << std::endl;

    vector<Unit> b;
    for(size_t it=0;it<phi.trials;++it)
    {
        Box box( it, 10, 100 );
        box.computeFor(phi,b);
        std::cerr << "b=" << b << std::endl;
    }
    
}
YOCTO_UNIT_TEST_DONE()
