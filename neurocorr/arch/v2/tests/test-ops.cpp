
#include "yocto/utest/run.hpp"
#include "yocto/code/rand.hpp"
#include "yocto/sys/timings.hpp"

#include "../boxes-ops.hpp"
#include "yocto/string/conv.hpp"

#define RUN_TIME 2.0

YOCTO_UNIT_TEST_IMPL(ops)
{
    size_t num_extra    = 0;
    size_t num_neurones = 2;
    size_t num_trials   = 1;
    size_t max_spikes   = 1000;
    bool   phi_perf     = true;
    
    if(argc>1) num_extra    = strconv::to<size_t>(argv[1],"num_extra");
    if(argc>2) num_neurones = strconv::to<size_t>(argv[2],"num_neurones");
    if(argc>3) num_trials   = strconv::to<size_t>(argv[3],"num_trials");
    if(argc>4) max_spikes   = strconv::to<size_t>(argv[4],"max_spikes");
    
    
    Crew     para(true);
    timings  tmx;
    
    
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
    PHI Phi(num_extra,records);
    
    //! computing phi for a given delta
    std::cerr << "Computing Phi, K=" << Phi.K << std::endl;
    std::cerr << "Dimension Per Neurone: " << Phi.dim        << std::endl;
    std::cerr << "Mixed Terms          : " << Phi.mixed.size << std::endl;
    
    const size_t delta = 4;
    
    if(phi_perf)
    {
        std::cerr << std::endl;
        std::cerr << "Computing Phi: SEQUENTIAL" << std::endl;
        YOCTO_TIMINGS(tmx,
                      RUN_TIME,
                      Phi.compute(delta,NULL)
                      );
        const double seqSpeedPhi = tmx.speed;
        std::cerr << "seqSpeedPhi=" << seqSpeedPhi << std::endl;
        
        std::cerr << "Computing Phi: PARALLEL" << std::endl;
        YOCTO_TIMINGS(tmx,
                      RUN_TIME,
                      Phi.compute(delta,&para)
                      );
        const double parSpeedPhi = tmx.speed;
        std::cerr << "parSpeedPhi=" << parSpeedPhi << std::endl;
        std::cerr << "\tSpeedUp_Phi=" << parSpeedPhi/seqSpeedPhi << std::endl;
    }
    else
    {
        Phi.compute(delta,&para);
    }
    //creating boxes: 3 per trials, 2 kinds
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
    
    // grouping by kind
    {
        std::cerr << std::endl;
        std::cerr << "Computing By Kind: SEQUENTIAL" << std::endl;
        const size_t num_matrices = boxes.assignIndices(GroupByKind);
        CMatrices    B(num_matrices,Phi.dim,Phi.neurones);
        CMatrices    G(num_matrices,Phi.dim,Phi.dim);
        YOCTO_TIMINGS(tmx,RUN_TIME,
                      BoxesOps::ComputeFor(boxes,Phi,B,G,NULL)
                      );
        const double seqSpeedOps = tmx.speed;
        std::cerr << "seqSpeedOps=" << seqSpeedOps << std::endl;
        B.ldz();
        G.ldz();
        
        std::cerr << "Computing By Kind: PARALLEL" << std::endl;
        YOCTO_TIMINGS(tmx,RUN_TIME,
                      BoxesOps::ComputeFor(boxes,Phi,B,G,&para)
                      );
        const double parSpeedOps = tmx.speed;
        std::cerr << "parSpeedOps=" << parSpeedOps << std::endl;
        std::cerr << "\tSpeedUp_Ops=" <<parSpeedOps/seqSpeedOps << std::endl;
    }
    
    //grouping by boxes
    {
        std::cerr << std::endl;
        std::cerr << "Computing By Boxes: SEQUENTIAL" << std::endl;
        const size_t num_matrices = boxes.assignIndices(GroupByBox);
        CMatrices    B(num_matrices,Phi.dim,Phi.neurones);
        CMatrices    G(num_matrices,Phi.dim,Phi.dim);
        YOCTO_TIMINGS(tmx,RUN_TIME,
                      BoxesOps::ComputeFor(boxes,Phi,B,G,NULL)
                      );
        const double seqSpeedOps = tmx.speed;
        std::cerr << "\tseqSpeedOps=" << seqSpeedOps << std::endl;
        B.ldz();
        G.ldz();
        
        std::cerr << "Computing By Boxes: PARALLEL" << std::endl;
        YOCTO_TIMINGS(tmx,RUN_TIME,
                      BoxesOps::ComputeFor(boxes,Phi,B,G,&para)
                      );
        const double parSpeedOps = tmx.speed;
        std::cerr << "parSpeedOps=" << parSpeedOps << std::endl;
        std::cerr << "\tSpeedUp_Ops=" <<parSpeedOps/seqSpeedOps << std::endl;
    }
    
    
    
}
YOCTO_UNIT_TEST_DONE()
