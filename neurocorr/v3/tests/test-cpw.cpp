#include "../cpw.hpp"
#include "../records.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/code/rand.hpp"

#include "yocto/ios/ocstream.hpp"

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


        {
            ios::wcstream fp("find.dat");
            for(Unit tau=pRec->tauMin-10;tau<=pRec->tauMax+10;++tau)
            {
                fp("%ld", long(tau));
                for(size_t len=1;len<=7;++len)
                {
                    size_t iStart = 0;
                    size_t count  = train.findIndicesWithin(tau, tau+len, iStart);
                    fp(" %u", unsigned(count));
                }

                fp("\n");
            }

        }

    }




    for(size_t iter=0;iter<10;++iter)
    {
        const size_t num_trials   = 2   + alea_leq(10);
        const size_t num_neurones = 2   + alea_leq(10);
        const size_t max_spikes   = 100 + alea_leq(1000);

        auto_ptr<Records> pRec( Records::CreateRandom(num_trials, num_neurones, max_spikes, 3) );
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
    
    
    
}
YOCTO_UNIT_TEST_DONE()
