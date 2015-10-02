#include "../records.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/code/rand.hpp"

#include "yocto/sys/wtime.hpp"

YOCTO_UNIT_TEST_IMPL(records)
{

    auto_ptr<Records> pRec( Records::CreateRandom(10, 5, 1000, 3) );
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
                        const size_t count = tr.findIndicesWithin_(tau,tauEnd,start);
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

#if 0
    // create intervals
    const size_t numBoxes = 100;
    vector<Unit> tauStart(numBoxes,as_capacity);
    vector<Unit> tauFinal(numBoxes,as_capacity);

    const Unit tau0 = records.tauMin - 5;
    const Unit w    = (records.tauMax - records.tauMin)/2;
    for(size_t i=0;i<numBoxes;++i)
    {
        tauStart.push_back( tau0 + Unit(alea_leq(w)));
        tauFinal.push_back( tauStart.back() +Unit(alea_leq(w)));
    }
#endif


}
YOCTO_UNIT_TEST_DONE()
