#include "../phi.hpp"
#include "yocto/utest/run.hpp"

YOCTO_UNIT_TEST_IMPL(phi)
{
    auto_ptr<Records> pRec( Records::GenerateRandom(5, 3, 100, 5) );
    Records &records = *pRec;

    PHI Phi(records,4);

    for(size_t j=1;j<=Phi.trials;++j)
    {
        for(size_t i=1;i<=Phi.neurones;++i)
        {
            for(size_t k=1;k<=Phi.K;++k)
            {
                const CPW &phi = Phi[j][i][k];
                std::cerr << "phi " << j << " " << i << " " << k << " capacity=" << phi.capacity() << std::endl;
            }
        }
    }

    threading::crew team(true);

    Phi.build(20,NULL);
    Phi.build(20,&team);
}
YOCTO_UNIT_TEST_DONE()
