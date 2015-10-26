#include "../vector-builder.hpp"
#include "../matrix-builder.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/code/rand.hpp"
#include "yocto/sys/wtime.hpp"

YOCTO_UNIT_TEST_IMPL(builders)
{

    threading::crew team(true);

    size_t neurones   = 4;
    size_t trials     = 5;
    size_t max_spikes = 100;
    size_t pace       = 5;
    size_t extra      = 2;


    size_t num_boxes = 3;
    size_t num_kinds = 2;


    uint64_t mark = 0;
    uint64_t seq_ticks = 0;
    uint64_t par_ticks = 0;

    uint64_t G_seq_ticks = 0;
    uint64_t G_par_ticks = 0;

    wtime    chrono;
    chrono.start();

    for(size_t iter=0;iter<8;++iter)
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

        UMatrices             seq_mu1(nm,Phi.dim,Phi.neurones);
        UMatrices             seq_mu2(nm,Phi.dim,Phi.neurones);

        UMatrices             par_mu1(nm,Phi.dim,Phi.neurones);
        UMatrices             par_mu2(nm,Phi.dim,Phi.neurones);

        UMatrices             seq_G(nm,Phi.dim,Phi.dim);
        UMatrices             par_G(nm,Phi.dim,Phi.dim);


        const Unit deltaMax = 10*pace;

        for(Unit delta=1;delta<=deltaMax;++delta)
        {
            seq_mu1.ldz();
            seq_mu2.ldz();

            par_mu1.ldz();
            par_mu2.ldz();

            seq_G.ldz();
            par_G.ldz();

            Phi.build(delta,&team);

            {
                mark = chrono.ticks();
                VectorBuilder vbuild(seq_mu1,seq_mu2,seq_muA,boxes,Phi,NULL);
                seq_ticks += chrono.ticks() - mark;
            }

            {
                mark = chrono.ticks();
                VectorBuilder vbuild(par_mu1,par_mu2,par_muA,boxes,Phi,&team);
                par_ticks += chrono.ticks() - mark;
            }

            {
                mark = chrono.ticks();
                MatrixBuilder mbuild(seq_G, boxes, Phi, NULL);
                G_seq_ticks += chrono.ticks() - mark;
            }

            {
                mark = chrono.ticks();
                MatrixBuilder mbuild(par_G, boxes, Phi, &team);
                G_par_ticks += chrono.ticks() - mark;
            }

            if(delta>=deltaMax)
            {
                for(size_t m=1;m<=nm;++m)
                {
                    std::cerr << "seq_mu1_" << m << "=" << seq_mu1[m] << std::endl;
                    std::cerr << "seq_mu2_" << m << "=" << seq_mu2[m] << std::endl;
                    std::cerr << "seq_muA_" << m << "=" << seq_muA[m] << std::endl;
                    std::cerr << "seq_G_"   << m << "=" << seq_G[m]   << std::endl;
                }
            }


            if(!areEqualMatrices(seq_mu1,par_mu1))
            {
                throw exception("mismatch mu1");
            }

            if(!areEqualMatrices(seq_mu2,par_mu2))
            {
                throw exception("mismatch mu2");
            }

            if(!areEqualMatrices(seq_muA,par_muA))
            {
                throw exception("mismatch muA");
            }


        }

        std::cerr << "boxes=" << boxes << std::endl;
    }


    std::cerr << "VectorBuilder:" << std::endl;
    {
        const double seq_time = chrono(seq_ticks);
        const double par_time = chrono(par_ticks);
        
        std::cerr << "\tseq_time=" << seq_time << std::endl;
        std::cerr << "\tpar_time=" << par_time << std::endl;
        std::cerr << "\tspeed_up=" << seq_time/par_time << std::endl;
    }

    std::cerr << std::endl;
    std::cerr << "MatrixBuilder:" << std::endl;
    {
        const double seq_time = chrono(G_seq_ticks);
        const double par_time = chrono(G_par_ticks);

        std::cerr << "\tseq_time=" << seq_time << std::endl;
        std::cerr << "\tpar_time=" << par_time << std::endl;
        std::cerr << "\tspeed_up=" << seq_time/par_time << std::endl;
    }
    
}
YOCTO_UNIT_TEST_DONE()
