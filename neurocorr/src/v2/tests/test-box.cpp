#include "../box.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/code/rand.hpp"

YOCTO_UNIT_TEST_IMPL(box)
{
    Crew     para;

    const size_t  num_neurones = 2   + alea_leq(50);
    const size_t  num_trials   = 3   + alea_leq(200);
    const size_t  max_spikes   = 100 + alea_leq(10000);

    std::cerr << "Creating Random Records" << std::endl;
    auto_ptr<Records> pRec( Records::CreateRandom(num_neurones, num_trials, max_spikes) );
    Records &records = *pRec;

    //! allocating memory
    std::cerr << "Allocating Memory for Phi" << std::endl;
    const size_t extra = 2;
    PHI phi(extra,records);

    //! computing phi for a given delta
    std::cerr << "Computing Phi..." << std::endl;
    phi.compute(1,&para);

    CMatrix<Unit> B(phi.neurones,1+phi.K);
    for(size_t it=0;it<phi.trials;++it)
    {
        Box box( it, 10, 200 );
        box.computeFor(phi,B);
        //std::cerr << "B=" << B << std::endl;
    }

}
YOCTO_UNIT_TEST_DONE()
