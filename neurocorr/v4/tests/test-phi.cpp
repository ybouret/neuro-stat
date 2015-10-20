#include "../phi.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/sys/timings.hpp"

YOCTO_UNIT_TEST_IMPL(phi)
{
    auto_ptr<Records> pRec( Records::GenerateRandom(5, 3, 100, 5) );
    Records &records = *pRec;
    const size_t extra = 4;
    PHI Phi(records,extra);
    PHI Phi_par(records,extra);


    threading::crew team(true);
#define PHI_DURATION 2.0
    timings tmx;
    std::cerr << "SeqTimings..." << std::endl;
    YOCTO_TIMINGS(tmx,PHI_DURATION,Phi.build(20,NULL));
    const double seqRate = tmx.speed;

    std::cerr << "ParTimings..." << std::endl;
    YOCTO_TIMINGS(tmx,PHI_DURATION,Phi_par.build(20,&team));
    const double parRate = tmx.speed;
    std::cerr << "seqRate=" << seqRate << std::endl;
    std::cerr << "parRate=" << parRate << std::endl;

    //! testing same result
    for(size_t j=1;j<=Phi.trials;++j)
    {
        for(size_t i=1;i<=Phi.neurones;++i)
        {
            for(size_t k=1;k<=Phi.K;++k)
            {
                const CPW &phi = Phi[j][i][k];
                if(phi!=Phi_par[j][i][k])
                {
                    throw exception("Mismatch!");
                }
            }
        }
    }

}
YOCTO_UNIT_TEST_DONE()
