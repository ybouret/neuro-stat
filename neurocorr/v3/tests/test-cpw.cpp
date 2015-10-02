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

        ios::wcstream fp("eval.dat");
        for(Unit tau=pRec->tauMin-5;tau<=pRec->tauMax+10;++tau)
        {
            fp("%ld %ld\n", long(tau), long( F(tau) ));
        }
    }


    
}
YOCTO_UNIT_TEST_DONE()
