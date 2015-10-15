#include "../train.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/code/rand.hpp"

YOCTO_UNIT_TEST_IMPL(train)
{

    for(size_t iter=0;iter<100;++iter)
    {
        const size_t  nr = 1+alea_leq(10);
        const size_t  ns = alea_lt(100);
        const size_t  nc = 1 + ns;

        std::cerr << "-- creating neuro data with " << nr << " rows" << std::endl;
        std::cerr << "-- max spikes=" << ns << std::endl;
        std::cerr << "-- columns   =" << nc << std::endl;
        CMatrix<Real> neurodata(nr,nc);
        for(size_t i=0;i<nr;++i)
        {
            const size_t nj = alea_leq(ns);
            std::cerr << "\ttrain #" << i << ": #spikes=" << nj << std::endl;
            neurodata[i][0] = nj;
            for(size_t j=1;j<=nj;++j)
            {
                neurodata[i][j] = j/100.0;
            }
            if(nj>1)
            {
                LArray<double> tr(&neurodata[i][1],nj);
                RQSort(tr);
            }
        }
        Trains trains(nr);
        trains.buildFrom(1000.0,neurodata,0);
    }

}
YOCTO_UNIT_TEST_DONE()

