#include "../phi.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/sys/wtime.hpp"


YOCTO_UNIT_TEST_IMPL(moments)
{

    threading::crew team(true);
    
    const size_t extra=1;
    for(size_t max_spikes=2;max_spikes<=512;max_spikes*=2)
    {
        std::cerr << std::endl;
        for(size_t iter=0;iter<1;++iter)
        {
            std::cerr << "\tmaxSpikes=" << max_spikes << std::endl;
            auto_ptr<Records> pRec( Records::GenerateRandom(2, 3, max_spikes, 5) );
            const Records    &records = *pRec;
            PHI Phi(records,extra);
            const Unit tauMin = records.minTau-10;
            const Unit tauMax = records.maxTau+10;
            const Unit wmax   = tauMax - tauMin;
            std::cerr << "\t\ttauMin=" << tauMin << ", tauMax=" << tauMax << std::endl;

            for(Unit delta=1;delta<=10;++delta)
            {
                Phi.build(delta,&team);

                for(Unit tauStart=tauMin;tauStart<=tauMax;++tauStart)
                {
                    for(Unit w=1;w<=wmax;++w)
                    {
                        const Unit   tauFinal = tauStart+w;

                        for(size_t j=1;j<=Phi.trials;++j)
                        {
                            for(size_t i=1;i<=Phi.neurones;++i)
                            {
                                const PHI_Functions &Phi_ji=Phi[j][i];
                                const UArray        *train = Phi_ji.train; assert(train);
                            }
                        }

                    }
                }
            }


        }

    }

}
YOCTO_UNIT_TEST_DONE()
