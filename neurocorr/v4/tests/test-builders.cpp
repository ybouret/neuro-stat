#include "../maxabs-builder.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/code/rand.hpp"
#include "yocto/sys/wtime.hpp"

YOCTO_UNIT_TEST_IMPL(builders)
{

    threading::crew team(true);

    size_t neurones   = 4;
    size_t trials     = 5;
    size_t max_spikes = 10000;
    size_t pace       = 5;
    size_t extra      = 5;


    size_t num_boxes = 3;
    size_t num_kinds = 2;


    uint64_t mark = 0;
    uint64_t maxabs_seq = 0;
    uint64_t maxabs_par = 0;
    wtime    chrono;
    chrono.start();

    {
        auto_ptr<Records> pRec( Records::GenerateRandom(neurones, trials, max_spikes, pace));
        const Records    &records = *pRec;
        const Unit        tauMin  = records.minTau-10*pace;
        const Unit        tauMax  = records.maxTau+10*pace;
        const Unit        width   = tauMax-tauMin;
        PHI Phi(records,extra);

        // create somes random boxes with alternate kinds
        matrix<Real> rboxes(4,num_boxes);
        for(size_t i=1;i<=num_boxes;++i)
        {
            rboxes[1][i] = 1+alea_lt(trials);
            rboxes[2][i] = tauMin + Unit(alea_leq(width))/4;
            rboxes[3][i] = rboxes[2][i] + 1+Unit(alea_leq(width))/2;
            rboxes[4][i] = 1+(i%num_kinds);
        }
        std::cerr << "BoxEst=" << rboxes << std::endl;
        Boxes boxes(1.0,rboxes);
        std::cerr << "Boxes=" << boxes << std::endl;

        const size_t          nm = boxes.assignIndices(GroupByKind);
        std::cerr << "#matrices=" << nm << std::endl;
        UMatrices             seq_muA(nm,Phi.dim,1);
        UMatrices             par_muA(nm,Phi.dim,1);

        const Unit deltaMax = 10*pace;
        for(Unit delta=1;delta<=deltaMax;++delta)
        {
            seq_muA.ldz();
            par_muA.ldz();
            Phi.build(delta,&team);
            {
                mark = chrono.ticks();
                MaxAbsBuilder mbuild(seq_muA, boxes, Phi, NULL);
                maxabs_seq += chrono.ticks() - mark;
            }
            if(delta>=deltaMax)
            {
                for(size_t m=1;m<=nm;++m)
                {
                    std::cerr << "seq_muA" << m << "=" << seq_muA[m] << std::endl;
                }
            }
            {
                mark = chrono.ticks();
                MaxAbsBuilder mbuild(par_muA, boxes, Phi, &team);
                maxabs_par += chrono.ticks() - mark;
            }
            if(!areEqualMatrices(seq_muA,par_muA))
            {
                throw exception("mismatch muA");
            }
        }
        std::cerr << "boxes=" << boxes << std::endl;
    }

    const double maxabs_seq_time = chrono(maxabs_seq);
    const double maxabs_par_time = chrono(maxabs_par);

    std::cerr << "maxabs_seq_time=" << maxabs_seq_time << std::endl;
    std::cerr << "maxabs_par_time=" << maxabs_par_time << std::endl;
    std::cerr << "maxabs_speed_up=" << maxabs_seq_time/maxabs_par_time << std::endl;
}
YOCTO_UNIT_TEST_DONE()
