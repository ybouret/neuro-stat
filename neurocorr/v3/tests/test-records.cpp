#include "../records.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/code/rand.hpp"

#include "yocto/sys/wtime.hpp"
#include "yocto/string/conv.hpp"

YOCTO_UNIT_TEST_IMPL(records)
{
    size_t trials  = 10;
    size_t neurons = 5;
    size_t spikes  = 1000;
    size_t pace    = 3;
    
    if(argc>1)
    {
        spikes = strconv::to<size_t>(argv[1],"spikes");
    }
    
    auto_ptr<Records> pRec( Records::CreateRandom(trials,neurons, spikes, pace) );
    Records &records = *pRec;
    records.display();
    wtime chrono;
    chrono.start();

   
    uint64_t tmx=0;
    uint64_t tmx_=0;
    uint64_t mark=0;
    // checking all possible lengths
    std::cerr << "Checking Find..." << std::endl;
    for(size_t j=0;j<records.trials;++j)
    {
        for(size_t i=0;i<records.neurones;++i)
        {
            const Train &tr = *(records[j][i]);
            const size_t np = tr.size();
            if(np>0)
            {
                const Unit tLo = tr[0]-5;
                const Unit tUp = tr[np-1]+5;
                for(Unit tau=tLo;tau<=tUp;++tau)
                {
                    for(Unit len=1;len<=10;++len)
                    {
                        const Unit   tauEnd = tau+len;

                        size_t       start = 0;
                        mark = chrono.ticks();
                        const size_t count = tr.findIndicesWithin(tau,tauEnd,start);
                        tmx += chrono.ticks()-mark;

                        size_t       start_ = 0;
                        mark = chrono.ticks();
                        const size_t count_ = tr.findIndicesWithin_(tau,tauEnd,start_);
                        tmx_ += chrono.ticks()-mark;
                        if(count!=count_)
                        {
                            throw exception("counts mismatch");
                        }
                        if(count>0)
                        {
                            if(start!=start_)
                                throw exception("start mismatch");
                        }
                    }
                }
            }
        }
    }
    
    const double optTime = chrono(tmx);
    const double rawTime = chrono(tmx_);
    std::cerr << "rawTime=" << rawTime << std::endl;
    std::cerr << "optTime=" << optTime << std::endl;
    

}
YOCTO_UNIT_TEST_DONE()
