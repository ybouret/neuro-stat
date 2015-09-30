#include "../phifn.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/code/rand.hpp"
#include "yocto/sys/wtime.hpp"

YOCTO_UNIT_TEST_IMPL(phifn)
{

    wtime chrono;
    chrono.start();
    uint64_t mark = 0;
    Crew     para;

    const size_t  num_neurones = 2 + alea_leq(50);
    const size_t  num_trials   = 3 + alea_leq(200);
    const size_t  max_spikes   = 100 + alea_leq(10000);

    auto_ptr<Records> pRec( Records::CreateRandom(num_neurones, num_trials, max_spikes) );
    Records &records = *pRec;
    records.displayNeurones();

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

   
}
YOCTO_UNIT_TEST_DONE()
