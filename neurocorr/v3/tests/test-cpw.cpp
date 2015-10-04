#include "../cpw.hpp"
#include "../records.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/code/rand.hpp"

#include "yocto/ios/ocstream.hpp"
#include "yocto/sys/wtime.hpp"

YOCTO_UNIT_TEST_IMPL(cpw)
{
    
    {
        CPW F(3);
        F.save("f0.dat");
        
        F.insert(1,1);
        F.save("f1.dat");
        
        F.foot = 1;
        F.free();
        F.insert(1,-1);
        F.insert(2,F.foot);
        F.save("f2.dat");
    }
    
    {
        CPW F;
        auto_ptr<Records> pRec( Records::CreateRandom(1, 1, 10, 3) );
        pRec->display();
        const Train &train = (*pRec)[0][0];
        train.save("train.dat");
        F.buildFrom(train,5);
        F.save("phi.dat");
        CPW G;
        G.shiftFrom(F,3);
        G.save("shr.dat");
        
        CPW P,Q;
        P.productOf(F,G);
        Q.productOf(G,F);
        P.save("p.dat");
        Q.save("q.dat");
        
        {
            ios::wcstream fp("eval.dat");
            for(Unit tau=pRec->tauMin-5;tau<=pRec->tauMax+10;++tau)
            {
                fp("%ld %ld\n", long(tau), long( F(tau) ));
            }
        }
    }
    
    
    
    std::cerr << "little trials..." << std::endl;
    for(size_t iter=0;iter<4;++iter)
    {
        const size_t num_trials   = 2   + alea_leq(10);
        const size_t num_neurones = 2   + alea_leq(10);
        const size_t max_spikes   = 100 + alea_leq(1000);
        
        auto_ptr<Records> pRec( Records::CreateRandom(num_trials,num_neurones,max_spikes,3) );
        CPW F,G,P;
        for(size_t j=0;j<pRec->trials;++j)
        {
            for(size_t i=0;i<pRec->neurones;++i)
            {
                const Train &lhs = (*pRec)[j][i];
                F.buildFrom(lhs,1+alea_leq(10));
                for(size_t jj=0;jj<pRec->trials;++jj)
                {
                    for(size_t ii=0;ii<pRec->neurones;++ii)
                    {
                        const Train &rhs = (*pRec)[jj][ii];
                        G.buildFrom(rhs,1+alea_leq(10));
                        P.productOf(F,G);
                    }
                }
            }
        }
    }
    
    uint64_t raw0 =0, opt0=0;
    uint64_t raw1 =0, opt1=0;
    uint64_t raw2 =0, opt2=0;
    uint64_t mark = 0;
    wtime chrono;
    chrono.start();
    
#define MARK()       mark = chrono.ticks()
#define CHRONO(val) val += chrono.ticks() - mark
    
    std::cerr << "Testing Moments" << std::endl;
    Moments rawM, optM;

    for(size_t iter=0;iter<100;++iter)
    {
        std::cerr << "."; std::cerr.flush();
        auto_ptr<Records> pRec;
        do
        {
            pRec.reset( Records::CreateRandom(1,1,100,3) );
        }
        while( (*pRec)(0).size() <= 0);
        Records     &records = *pRec;
        const Train &train   = records(0);
        const Unit   tIni    = train[0];
        const Unit   tEnd    = train[train.size()-1];
        const Unit   dT      = tEnd - tIni + 1;
        const Unit   wMax    = 50;
        const Unit   tLo     = tIni - wMax-2;
        const Unit   tUp     = tEnd+1;
        const Unit   tMid    = (tIni+tEnd)/2;

        CPW F(10);
        
        for(Unit tau=tLo;tau<=tUp;++tau)
        {
            for(Unit len=1;len<=wMax;++len)
            {
                const Unit tauEnd = tau+len;
                size_t       offset = 0;
                const size_t length = train.findIndicesWithin(tau, tauEnd, offset);
                {
                    size_t       offset_ = 0;
                    const size_t length_ = train.findIndicesWithin_(tau, tauEnd, offset_);
                    if(length_!=length)
                    {
                        throw exception("CPW: length mismatch!");
                    }
                    if(length>0&& (offset!=offset_) )
                    {
                        throw exception("CPW: offset mismatch!");
                    }
                }
                
                // level 0
                F.free();
                F.foot = 3;
                
                MARK();
                F.evalSumOn_(train, length, offset, rawM);
                CHRONO(raw0);
                MARK();
                F.evalSumOn(train,length,offset,optM);
                CHRONO(opt0);
                if(rawM!=optM)
                {
                    throw exception("Moments Mismatch @Level-0");
                }
                
                
                // level 1
                F.free();
                F.foot = 2;
                F.insert(tMid, 3);
                MARK();
                F.evalSumOn_(train, length, offset, rawM);
                CHRONO(raw1);
                MARK();
                F.evalSumOn(train,length,offset,optM);
                CHRONO(opt1);
                if(rawM!=optM)
                {
                    throw exception("Moments Mismatch@Level-1");
                }

                // level 2
                F.free();
                F.foot = 2;
                F.insert(tMid-dT/4,3);
                F.insert(tMid+dT/4,5);
                
                MARK();
                F.evalSumOn_(train, length, offset, rawM);
                CHRONO(raw2);
                MARK();
                F.evalSumOn(train,length,offset,optM);
                CHRONO(opt2);
                if(rawM!=optM)
                {
                    std::cerr << "raw=" << rawM << ", opt=" << optM << std::endl;
                    throw exception("Moments Mismatch @Level-2");
                }

            }
        }
        
        
        
    }
    std::cerr << std::endl;
    const double raw0Time = chrono(raw0);
    const double opt0Time = chrono(opt0);
    std::cerr << "raw0Time=" << raw0Time << std::endl;
    std::cerr << "opt0Time=" << opt0Time << std::endl;
    
    std::cerr << std::endl;
    const double raw1Time = chrono(raw1);
    const double opt1Time = chrono(opt1);
    std::cerr << "raw1Time=" << raw1Time << std::endl;
    std::cerr << "opt1Time=" << opt1Time << std::endl;
    
    std::cerr << std::endl;
    const double raw2Time = chrono(raw2);
    const double opt2Time = chrono(opt2);
    std::cerr << "raw2Time=" << raw2Time << std::endl;
    std::cerr << "opt2Time=" << opt2Time << std::endl;
    
    return 0;
    
}
YOCTO_UNIT_TEST_DONE()
