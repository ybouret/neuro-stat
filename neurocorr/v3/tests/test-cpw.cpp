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
        const Train &train = *(*pRec)[0][0];
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
                const Train &lhs = *(*pRec)[j][i];
                F.buildFrom(lhs,1+alea_leq(10));
                for(size_t jj=0;jj<pRec->trials;++jj)
                {
                    for(size_t ii=0;ii<pRec->neurones;++ii)
                    {
                        const Train &rhs = *(*pRec)[jj][ii];
                        G.buildFrom(rhs,1+alea_leq(10));
                        P.productOf(F,G);
                    }
                }
            }
        }
    }
    
    uint64_t raw =0, opt=0;
    uint64_t mark = 0;
    wtime chrono;
    chrono.start();
    
    std::cerr << "Testing Sums" << std::endl;
    
    for(size_t iter=0;iter<10;++iter)
    {
        auto_ptr<Records> pRec( Records::CreateRandom(1,1,100,3) );
        CPW F(100);
        const Train &train = *(*pRec)[0][0];
        if(train.size())
        {
            
            const Unit   tLo = train[0]-5;
            const Unit   tUp = train[train.size()-1]+1;
            const Unit   tMid = tLo+ (tUp-tLo)/2;
            for(Unit tau=tLo;tau<=tUp;++tau)
            {
                for(Unit len=1;len<=50;++len)
                {
                    const Unit   tauEnd = tau+len;
                    size_t       offset = 0;
                    const size_t length = train.findIndicesWithin(tau,tauEnd, offset);
                    // constant expression
                    F.free();
                    F.foot = 1;
                    
                    {
                        mark = chrono.ticks();
                        const Unit rawSum = F.evalSumOn_(train, length, offset);
                        raw += chrono.ticks()-mark;
                        
                        const Unit optSum = F.evalSumOn(train, length, offset);
                        opt += chrono.ticks()-mark;
                        if(optSum!=rawSum)
                        {
                            throw exception("Mismatch results, Level 0");
                        }
                    }
                    
                    F.insert(tMid,2); assert(1==F.size);
                    {
                        mark = chrono.ticks();
                        const Unit rawSum = F.evalSumOn_(train, length, offset);
                        raw += chrono.ticks()-mark;
                        
                        const Unit optSum = F.evalSumOn(train, length, offset);
                        opt += chrono.ticks()-mark;
                        if(optSum!=rawSum)
                        {
                            throw exception("Mismatch results, Level 1");
                        }
                    }
                    
                }
                
            }
            continue;
            
            if(false)
            {
                F.buildFrom(train,5);
                for(Unit tau=tLo;tau<=tUp;++tau)
                {
                    for(Unit len=1;len<=50;++len)
                    {
                        const Unit   tauEnd = tau+len;
                        size_t       offset = 0;
                        const size_t length = train.findIndicesWithin(tau,tauEnd, offset);
                        {
                            size_t offset_ = 0;
                            const size_t length_ = train.findIndicesWithin_(tau,tauEnd, offset_);
                            if(length_!=length)
                            {
                                throw exception("Length Mismatch!");
                            }
                            if(length>0 && offset!=offset_) throw exception("Offset Mismatch!");
                        }
                        mark = chrono.ticks();
                        const Unit rawSum = F.evalSumOn_(train, length, offset);
                        raw += chrono.ticks()-mark;
                        
                        mark = chrono.ticks();
                        const Unit optSum = F.evalSumOn(train, length, offset);
                        opt += chrono.ticks()-mark;
                        if(optSum!=rawSum)
                        {
                            throw exception("Mismatch results!");
                        }
                    }
                }
                
            }
            
        }
    }
    const double rawTime = chrono(raw);
    const double optTime = chrono(opt);
    std::cerr << "rawTime=" << rawTime << std::endl;
    std::cerr << "optTime=" << optTime << std::endl;
    
    
    
}
YOCTO_UNIT_TEST_DONE()
