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
    std::cerr << "records.trials   =" << records.trials   << std::endl;
    std::cerr << "records.neurons  =" << records.neurones << std::endl;
    std::cerr << "records.trains   =" << records.items    << std::endl;
    std::cerr << "records.maxCount =" << records.maxCount << std::endl;

    //! allocating memory
    std::cerr << "Allocating Memory for Phi" << std::endl;
    const size_t extra = 2;
    PHI Phi(extra,records);

    //! computing phi for a given delta
    std::cerr << "Computing Phi..." << std::endl;
    Phi.compute(1,&para);


    std::cerr << "Computing B's and G's" << std::endl;
    const size_t  num_rows=1+Phi.neurones*Phi.K;
    const size_t  num_cols=Phi.neurones;
    CMatrix<Unit> B(num_rows,num_cols);
    CMatrix<Unit> G(num_rows,num_rows);

    UList        Tau;//( records.maxCount, as_capacity );
    const size_t itOut = Phi.trials/2;
    for(size_t it=0;it<Phi.trials;++it)
    {
        Box box( it, 10, 200 );
        box.computeRHS(Phi,B,Tau);
        box.computeMATRIX(Phi,G);
        if(it==itOut)
        {
            std::cerr << "B=" << B << std::endl;
            std::cerr << "G=" << G << std::endl;
        }
    }


}
YOCTO_UNIT_TEST_DONE()
