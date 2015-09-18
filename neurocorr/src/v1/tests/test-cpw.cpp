#include "../cpw-function.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/ios/ocstream.hpp"
#include "yocto/code/rand.hpp"
#include "../neurodata.hpp"

YOCTO_UNIT_TEST_IMPL(cpw)
{
    {
        CPWFunction F;

        for(size_t i=1+alea_leq(10);i>0;--i)
        {
            F.insert( alea<double>(), (alea<double>()-0.5) * 10.0 );
        }

        F.foot = alea<double>();

        ios::wcstream fp("step.dat");
        for(double t=0;t<=1;t+=0.001)
        {
            fp("%g %g\n", t, F(t));
        }
    }

    {

        const size_t nr = 1;// + alea_leq(20);
        const size_t nt = 1;// + alea_leq(100);
        const size_t ni = 1 + alea_leq(50);


        NeuroData ND(nr,nt,ni);
        std::cerr << std::endl;
        std::cerr << "\t(*) #neurons=" << ND.neurons << std::endl;
        std::cerr << "\t(*) #trials =" << ND.trials  << std::endl;
        std::cerr << "\t(*) #trains =" << ND.trains  << std::endl;
        std::cerr << "\t(*) #maxinp =" << ND.max_spikes_per_train  << std::endl;

        const double tmax = 10.0;
        for(size_t i=0;i<ND.trains;++i)
        {
            RArray<double> &input = ND.get_raw_input(i);
            const size_t ns = alea_leq(ni);
            input[0] = ns;
            assert(ns<=input.size()+1);
            for(size_t i=1;i<=ns;++i)
            {
                input[i] = alea<double>() * tmax;
            }
            Record rec(input);
            RQSort(rec);
        }
        ND.setup();
        for(size_t i=0;i<ND.neurons;++i)
        {

            const Neuron &n = ND.neuron[i];
            std::cerr << "Neuron[" << i << "] : #trials=" << n.trials << std::endl;
        }
        const Record &train = ND.neuron[0][0];
        {
            ios::wcstream fp("train.dat");
            for(size_t i=0;i<train.size();++i)
            {
                fp("%g 0\n", train[i]);
            }
        }
        CPWFunction   Phi1;
        const double  delta = 1;
        Phi1.buildFromFull(train, delta);

        if(Phi1.size()>1)
        {
            ios::wcstream fp("phi1.dat");
            //fp("%g %g\n", Phi1[1].t-delta,Phi1.foot);
            fp("%g %g\n", Phi1[1].t,Phi1.foot);
            for(size_t i=1;i<Phi1.size();++i)
            {
                const Coord &C0 = Phi1[i];
                const Coord &C1 = Phi1[i+1];
                fp("%g %g\n", C0.t, C0.v);
                fp("%g %g\n", C1.t, C0.v);
            }
            const Coord &C = Phi1[Phi1.size()];
            fp("%g %g\n", C.t, C.v);
        }
    }
}
YOCTO_UNIT_TEST_DONE()
