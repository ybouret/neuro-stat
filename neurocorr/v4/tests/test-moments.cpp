#include "../phi.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/sys/timings.hpp"
#include "../locate.hpp"

#define DURATION 0.05

static inline
void full_test( const CPW &F, const UArray &tau )
{
    timings tmx;
    Moments raw;
    Moments opt;

    YOCTO_TIMINGS(tmx,DURATION,F.evalMoments_(tau, 1, tau.size(), raw));
    const double rawSpeed = tmx.speed/1e6;
    YOCTO_TIMINGS(tmx,DURATION,F.evalMoments (tau, 1, tau.size(), opt));
    const double optSpeed = tmx.speed/1e6;

    //std::cerr << "Raw=" << raw << std::endl;
    //std::cerr << "Opt=" << opt << std::endl;
    if(raw!=opt)
    {
        throw exception("moments failure!");
    }
    std::cerr << "Speed: " << rawSpeed << " -> " << optSpeed << " (x " << optSpeed/rawSpeed << ")" << std::endl;
}

YOCTO_UNIT_TEST_IMPL(moments)
{

    threading::crew team(true);
    
    const size_t extra=1;
    Moments      raw;
    Moments      opt;

    // dummy cases
    {
        CPW F;
        F.foot = 1;
        UVector tau;
        for(Unit i=-5;i<=5;++i) tau.push_back(i);

        full_test(F,tau);

        F.add(-6,2);
        full_test(F,tau);

        F.free();
        F.add(5,2);
        full_test(F,tau);

        F.free();
        F.add(1,2);
        full_test(F,tau);

        for(Unit i=-10;i<=10;++i)
        {
            F.free();
            F.add(i,2);
            full_test(F,tau);
        }


    }


    for(size_t max_spikes=2;max_spikes<=128;max_spikes*=2)
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
                std::cerr << delta << ".";
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

                                size_t ip = 0;
                                size_t np = Locate::IndicesWithin(*train, tauStart, tauFinal, ip);

                                for(size_t k=1;k<=Phi.K;++k)
                                {
                                    const CPW    &phi = Phi_ji[k];
                                    phi.evalMoments_(*train,ip,np,raw);
                                    phi.evalMoments( *train,ip,np,opt);
                                    if(raw==opt)
                                    {
                                        std::cerr << "+";
                                    }
                                    else
                                    {
                                        std::cerr << "-";
                                    }
                                }

                            }
                        }

                    }
                }
            }
            std::cerr << std::endl;


        }

    }

}
YOCTO_UNIT_TEST_DONE()
