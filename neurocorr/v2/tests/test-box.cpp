#include "../boxes.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/code/rand.hpp"
#include "yocto/sys/wtime.hpp"

YOCTO_UNIT_TEST_IMPL(box)
{

    Crew     para;
    uint64_t mark=0;

    wtime    chrono;
    chrono.start();

    const size_t  num_neurones = 5;// + alea_leq(100);
    const size_t  num_trials   = 3;// + alea_leq(100);
    const size_t  max_spikes   = 1000 + alea_leq(2000);
    const size_t  extra        = 2;

    std::cerr << "Creating Random Records" << std::endl;
    auto_ptr<Records> pRec( Records::CreateRandom(num_neurones, num_trials, max_spikes) );
    Records &records = *pRec;
    std::cerr << "records.trials   =" << records.trials   << std::endl;
    std::cerr << "records.neurons  =" << records.neurones << std::endl;
    std::cerr << "records.trains   =" << records.items    << std::endl;
    std::cerr << "records.maxCount =" << records.maxCount << std::endl;
    std::cerr << "records.tauMin   =" << records.tauMin   << std::endl;
    std::cerr << "records.tauMax   =" << records.tauMax   << std::endl;

    //! allocating memory
    std::cerr << "Allocating Memory for Phi" << std::endl;
    PHI Phi(extra,records);

    //! computing phi for a given delta
    std::cerr << "Computing Phi..." << std::endl;
    Phi.compute(4,&para);

    std::cerr << "There are " << Phi.mixed.size << " mixed terms" << std::endl;
    std::cerr << "For a dimension of " << Phi.dim << std::endl;

    std::cerr << "Computing B's and G's" << std::endl;
    //const size_t  num_rows= Phi.dim;
    //const size_t  num_cols= Phi.neurones;
    //CMatrix<Unit> B(num_rows,num_cols);
    //CMatrix<Unit> G(num_rows,num_rows);

    const size_t boxes_per_trials = 3;
    Boxes boxes(1.0,boxes_per_trials*num_trials);
    const Unit maxLength = records.tauMax-records.tauMin + 1;
    for(size_t j=0,k=0;j<num_trials;++j)
    {
        for(size_t i=0;i<boxes_per_trials;++i)
        {
            const Unit ini = alea_leq(maxLength);
            const Unit len = 1 + alea_leq(maxLength);
            const Box  b((++k)%num_trials,ini,ini+len);
            (Box::Kind &)(b.kind) = (i%2);
            boxes.push_back(b);
        }
    }
    std::cerr << "Boxes:" << std::endl;
    for(size_t i=0;i<boxes.size;++i)
    {
        std::cerr << boxes[i] << std::endl;
    }

    const size_t nG = boxes.assignIndices(GroupByKind);
    CMatrices G(nG,Phi.dim,Phi.dim);

    // sequential trial
    std::cerr << "Sequential Computation" << std::endl;
    mark = chrono.ticks();
    MixedEvaluator(boxes,Phi, G, NULL);
    const double seqMixed = chrono( chrono.ticks() - mark );
    std::cerr << "seqMixedTime=" << seqMixed << std::endl;
    for(size_t i=0;i<G.size;++i)
    {
        std::cerr << "seqG" << i << "=" << *G[i] << std::endl;
    }


    // parallel code
    std::cerr << "Parallel Computation of Mixed terms" << std::endl;
    for(size_t i=0;i<G.size;++i)
    {
        G[i]->ld(0);
    }
    mark = chrono.ticks();
    MixedEvaluator(boxes,Phi, G,&para);
    const double parMixed = chrono( chrono.ticks() - mark );
    std::cerr << "parMixedTime=" << parMixed << std::endl;
    for(size_t i=0;i<G.size;++i)
    {
        std::cerr << "parG" << i << "=" << *G[i] << std::endl;
    }

    std::cerr << "seqMixedTime=" << seqMixed << std::endl;
    std::cerr << "parMixedTime=" << parMixed << std::endl;
    std::cerr << "SpeedUp     =" << seqMixed/parMixed << std::endl;
    

#if 0
    uint64_t Bcount=0;
    uint64_t Gcount=0;
    UList        Tau;//( records.maxCount, as_capacity );
    const size_t itOut = Phi.trials/2;
    for(size_t it=0;it<Phi.trials;++it)
    {
        Box box( it, 0, 500 );
        mark = chrono.ticks();
        box.appendRHS(Phi,B,Tau);
        Bcount += chrono.ticks()-mark;

        mark = chrono.ticks();
        //box.computeMATRIX(Phi,G);
        box.appendLinearTo(G,Phi);
        Gcount += chrono.ticks()-mark;
        if(it==itOut)
        {
            //std::cerr << "B=" << B << std::endl;
            //std::cerr << "G=" << G << std::endl;
        }
    }
    const double Bell = chrono(Bcount);
    const double Gell = chrono(Gcount);
    std::cerr << "Bell=" << Bell << std::endl;
    std::cerr << "Gell=" << Gell << std::endl;
#endif


}
YOCTO_UNIT_TEST_DONE()
