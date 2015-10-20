#include "../records.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/sys/wtime.hpp"
YOCTO_UNIT_TEST_IMPL(locate)
{

    wtime chrono;
    chrono.start();

    uint64_t raw64 = 0;
    uint64_t chk64 = 0;
    uint64_t opt64 = 0;
    uint64_t mark  = 0;
    for(size_t max_spikes=2;max_spikes<=512;max_spikes*=2)
    {
        std::cerr << "maxSpikes=" << max_spikes << std::endl;
        auto_ptr<Records> pRec( Records::GenerateRandom(1, 1, max_spikes, 5) );
        const Records    &records = *pRec;
        const Train      &train   = records[1][1];

        const Unit tauMin = records.minTau-10;
        const Unit tauMax = records.maxTau+10;
        const Unit wmax   = tauMax - tauMin;
        std::cerr << "tauMin=" << tauMin << ", tauMax=" << tauMax << std::endl;
        size_t iraw=0;
        size_t iopt=0;
        size_t ichk=0;
        for(Unit tauStart=tauMin;tauStart<=tauMax;++tauStart)
        {
            for(Unit w=1;w<=wmax;++w)
            {
                const Unit   tauFinal = tauStart+w;
                mark = chrono.ticks();
                const size_t nraw     = train.locateIndicesWithin_(tauStart,tauFinal, iraw);
                raw64 += chrono.ticks() - mark;


                mark = chrono.ticks();
                bool   init = true;
                size_t nchk = 0;
                for(size_t i=1;i<=train.size();++i)
                {
                    const Unit tmp = train[i];
                    if(tauStart<tmp&&tmp<=tauFinal)
                    {
                        ++nchk;
                        if(init)
                        {
                            ichk = i;
                            init = false;
                        }
                    }
                }
                chk64 += chrono.ticks()-mark;

                if(nchk!=nraw)
                {
                    throw exception("Invalid Raw Counting : nraw=%u, nchk=%u!", unsigned(nraw), unsigned(nchk) );
                }

                if(nchk>0&&ichk!=iraw)
                {
                    throw exception("Invalid Raw Starting Point!");
                }

                mark = chrono.ticks();
                const size_t nopt     = train.locateIndicesWithin(tauStart,tauFinal,iopt);
                opt64 += chrono.ticks()-mark;


                if(nraw!=nopt)
                {
                    throw exception("Invalid Opt Counting: nraw=%u, nopt=%u!",unsigned(nraw),unsigned(nopt));
                }

                if(nopt>0&&ichk!=iopt)
                {
                    throw exception("Invalid Opt Starting Point!");
                }
            }
        }

    }

    const double tchk = chrono(chk64);
    const double traw = chrono(raw64);
    const double topt = chrono(opt64);
    std::cerr << "tchk=" << tchk << std::endl;
    std::cerr << "traw=" << traw << std::endl;
    std::cerr << "topt=" << topt << std::endl;

    
    
}
YOCTO_UNIT_TEST_DONE()

