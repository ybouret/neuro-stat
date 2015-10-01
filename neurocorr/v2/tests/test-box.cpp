#include "../boxes-mixed.hpp"
#include "../boxes-linear.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/code/rand.hpp"
#include "yocto/sys/wtime.hpp"

static
double RunEvalMixed(const Boxes &boxes,
                      const PHI   &Phi,
                      Matrices    &G,
                      Crew        *para,
                      wtime       &chrono)
{
    const uint64_t mark = chrono.ticks();
    MixedEvaluator(boxes,Phi, G, para);
    return chrono(chrono.ticks() - mark);
}

static
double RunEvalLinear(const Boxes &boxes,
                     const PHI   &Phi,
                     Matrices    &B,
                     Crew        *para,
                     wtime       &chrono)
{
    const uint64_t mark = chrono.ticks();
    LinearEvaluator(boxes,Phi, B, para);
    return chrono(chrono.ticks() - mark);
}

YOCTO_UNIT_TEST_IMPL(box)
{

    Crew     para;
    wtime    chrono;
    chrono.start();

    const size_t  num_neurones = 5;// + alea_leq(100);
    const size_t  num_trials   = 13;// + alea_leq(100);
    const size_t  max_spikes   = 1000 + alea_leq(2000);
    const size_t  extra        = 4;

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

    std::cerr << "Creating Boxes" << std::endl;
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
    CMatrices B(nG,Phi.dim,Phi.neurones);
    
    std::cerr << std::endl;
    std::cerr << "\t\tLINEAR TERMS" << std::endl;
    B.ldz();
    std::cerr << "Sequential Computation of Linear Terms" << std::endl;
    const double seqLinear = RunEvalLinear(boxes, Phi, B, NULL, chrono);
    for(size_t i=0;i<B.size;++i)
    {
        assert(B[i]);
        std::cerr << "seqB" << i << "=" << *B[i] << std::endl;
    }
    std::cerr << "seqLinear=" << seqLinear << std::endl;
    
    std::cerr << "Parallel Computation of Linear Terms" << std::endl;
    B.neg();
    const double parLinear = RunEvalLinear(boxes, Phi, B, &para, chrono);
    for(size_t i=0;i<B.size;++i)
    {
        assert(B[i]);
        std::cerr << "parB" << i << "=" << *B[i] << std::endl;
    }
    std::cerr << "parLinear=" << parLinear << std::endl;
    std::cerr << "seqLinear=" << seqLinear << std::endl;
    std::cerr << "SpeedUpLinear=" << seqLinear / parLinear << std::endl;

    
    
    std::cerr << std::endl;
    std::cerr << "\t\tMIXED TERMS" << std::endl;
    // sequential trial
    std::cerr << "Sequential Computation of Mixed Terms" << std::endl;
    const double seqMixed = RunEvalMixed(boxes, Phi, G, NULL, chrono);
    for(size_t i=0;i<G.size;++i)
    {
        std::cerr << "seqG" << i << "=" << *G[i] << std::endl;
    }
    std::cerr << "seqMixed=" << seqMixed << std::endl;

    // parallel code
    std::cerr << "Parallel Computation of Mixed terms" << std::endl;
    G.neg();
    const double parMixed = RunEvalMixed(boxes, Phi, G, &para, chrono);
    for(size_t i=0;i<G.size;++i)
    {
        std::cerr << "parG" << i << "=" << *G[i] << std::endl;
    }
    std::cerr << "parMixed=" << parMixed << std::endl;
    std::cerr << "seqMixed=" << seqMixed << std::endl;
    std::cerr << "SpeedUpMixed = " << seqMixed/parMixed << std::endl;
    
    
    std::cerr << std::endl;
    std::cerr << "Summary: " << std::endl;
    std::cerr << "SpeedUpLinear = " << seqLinear / parLinear << std::endl;
    std::cerr << "SpeedUpMixed  = " << seqMixed/parMixed << std::endl;

    

}
YOCTO_UNIT_TEST_DONE()
