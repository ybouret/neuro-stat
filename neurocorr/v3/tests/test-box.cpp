#include "../boxes.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/code/rand.hpp"

#include "yocto/ios/ocstream.hpp"
#include "yocto/sys/wtime.hpp"

#include "yocto/string/conv.hpp"
#include "yocto/sys/timings.hpp"

#include "../vector-builder.hpp"
#include "../matrix-builder.hpp"

static inline
void test_vector_values( const Boxes &boxes,PHI &Phi, const Grouping group, Crew &para )
{
    const size_t num_matrices = boxes.assignIndices(group);
    std::cerr << "Allocating " << num_matrices <<  " matrices, " << Phi.dim << " x " << Phi.neurones << std::endl;
    MatricesOf<Unit,CMatrix> mu1(num_matrices,Phi.dim,Phi.neurones);
    MatricesOf<Unit,CMatrix> mu2(num_matrices,Phi.dim,Phi.neurones);
    MatricesOf<Unit,CMatrix> muA(num_matrices,Phi.dim,Phi.neurones);

    mu1.ld(0);
    mu2.ld(0);
    muA.ld(0);

    {
        VectorBuilder vbuild(mu1,mu2,muA,boxes,Phi,NULL);
    }

    for(size_t i=0;i<num_matrices;++i)
    {
        std::cerr << "mu1_" << i << "=" << mu1[i] << std::endl;
    }

    for(size_t i=0;i<num_matrices;++i)
    {
        std::cerr << "mu2_" << i << "=" << mu2[i] << std::endl;
    }

    for(size_t i=0;i<num_matrices;++i)
    {
        std::cerr << "muA_" << i << "=" << muA[i] << std::endl;
    }

    mu1.neg();
    mu2.neg();
    muA.ld(0);
    {
        VectorBuilder vbuild(mu1,mu2,muA,boxes,Phi,&para);
    }
    for(size_t i=0;i<num_matrices;++i)
    {
        std::cerr << "mu1_" << i << "=" << mu1[i] << std::endl;
    }

    for(size_t i=0;i<num_matrices;++i)
    {
        std::cerr << "mu2_" << i << "=" << mu2[i] << std::endl;
    }

    for(size_t i=0;i<num_matrices;++i)
    {
        std::cerr << "muA_" << i << "=" << muA[i] << std::endl;
    }

}

static inline
void test_vector_perfs( const Boxes &boxes,PHI &Phi, const Grouping group, Crew &para )
{
    std::cerr << std::endl << "\t<VectorPerfs>" << std::endl;
    const size_t num_matrices = boxes.assignIndices(group);
    std::cerr << "Allocating " << num_matrices <<  " matrices, " << Phi.dim << " x " << Phi.neurones << std::endl;
    MatricesOf<Unit,CMatrix> mu1(num_matrices,Phi.dim,Phi.neurones);
    MatricesOf<Unit,CMatrix> mu2(num_matrices,Phi.dim,Phi.neurones);
    MatricesOf<Unit,CMatrix> muA(num_matrices,Phi.dim,Phi.neurones);
    {

        // Testing performances
        std::cerr << std::endl << "Performances..." << std::endl;
#define PERF_DURATION 3.0
        timings tmx;
        YOCTO_TIMINGS(tmx, PERF_DURATION, VectorBuilder vbuild(mu1,mu2,muA,boxes,Phi,NULL) );
        const double seqSpeed = tmx.speed;
        std::cerr << "seqSpeed=" << seqSpeed << std::endl;
        YOCTO_TIMINGS(tmx, PERF_DURATION, VectorBuilder vbuild(mu1,mu2,muA,boxes,Phi,&para) );
        const double parSpeed = tmx.speed;
        std::cerr << "parSpeed=" << parSpeed << std::endl;
        std::cerr << "SpeedUp =" << parSpeed/seqSpeed  << std::endl;
    }
    std::cerr << "\t</VectorPerfs>" << std::endl << std::endl ;

}


static inline
void test_matrix_values(const Boxes &boxes,PHI &Phi, const Grouping group, Crew &para )
{
    const size_t num_matrices = boxes.assignIndices(group);
    std::cerr << "Allocating " << num_matrices <<  " matrices, " << Phi.dim << " x " << Phi.dim << std::endl;
    MatricesOf<Unit,CMatrix> MG(num_matrices,Phi.dim,Phi.dim);

    { MatrixBuilder mbuild(MG,boxes,Phi,NULL); }
    for(size_t i=0;i<num_matrices;++i)
    {
        std::cerr << "G" << i << "=" << MG[i] << std::endl;
    }

    MG.neg();
    { MatrixBuilder mbuild(MG,boxes,Phi,NULL);}

    for(size_t i=0;i<num_matrices;++i)
    {
        std::cerr << "G" << i << "=" << MG[i] << std::endl;
    }

}

static inline
void test_matrix_perfs(const Boxes &boxes,PHI &Phi, const Grouping group, Crew &para )
{
    std::cerr << std::endl << "\t<MatrixPerfs>" << std::endl;
    const size_t num_matrices = boxes.assignIndices(group);
    std::cerr << "Allocating " << num_matrices <<  " matrices, " << Phi.dim << " x " << Phi.dim << std::endl;
    MatricesOf<Unit,CMatrix> MG(num_matrices,Phi.dim,Phi.dim);

#define PERF_DURATION 3.0
    timings tmx;
    YOCTO_TIMINGS(tmx, PERF_DURATION, MatrixBuilder mbuild(MG,boxes,Phi,NULL) );
    const double seqSpeed = tmx.speed;
    std::cerr << "seqSpeed=" << seqSpeed << std::endl;
    YOCTO_TIMINGS(tmx, PERF_DURATION, MatrixBuilder mbuild(MG,boxes,Phi,&para)  );
    const double parSpeed = tmx.speed;
    std::cerr << "parSpeed=" << parSpeed << std::endl;
    std::cerr << "SpeedUp =" << parSpeed/seqSpeed  << std::endl;


    std::cerr  << "\t</MatrixPerfs>" << std::endl << std::endl;

}


YOCTO_UNIT_TEST_IMPL(box)
{
    Crew para(true);

    size_t trials   = 5;    if( argc>1 ) trials   = strconv::to<size_t>(argv[1],"trials");
    size_t neurones = 8;    if( argc>2 ) neurones = strconv::to<size_t>(argv[2],"neurones");
    size_t spikes   = 1000; if( argc>3 ) spikes   = strconv::to<size_t>(argv[3],"spikes");
    size_t extra    = 5;    if( argc>4 ) extra    = strconv::to<size_t>(argv[4],"extra");

    auto_ptr<Records> pRec( Records::CreateRandom(trials,neurones,spikes, 3) );
    Records &records = *pRec;
    records.display();

    std::cerr << "Allocating Memory for Phi" << std::endl;
    PHI Phi(records,extra);
    std::cerr << "Phi.K="   << Phi.K   << std::endl;
    std::cerr << "Phi.dim=" << Phi.dim << std::endl;

    std::cerr << "Computing Phi..." << std::endl;
    const Unit delta = 5;
    Phi.compute(delta, &para);

    // make 3 boxes, 2 kinds
    const size_t boxes_per_trials = 3;
    const size_t nb               = trials*boxes_per_trials;
    Boxes boxes(records.scale,nb);
    const Unit dT = records.tauMax-records.tauMin+1;
    for(size_t j=0;j<trials;++j)
    {
        for(size_t b=0;b<boxes_per_trials;++b)
        {
            const Unit      tauStart  = records.tauMin + alea_leq(dT/2);
            const Unit      tauFinal  = tauStart + 1 + alea_leq(dT/2);
            const Box::Kind kind      = boxes.size%2;
            const Box box(j,tauStart,tauFinal,kind);
            boxes.push_back(box);
        }
    }

    if(false)
    {
        test_vector_values(boxes, Phi, GroupByKind, para);
        test_matrix_values(boxes, Phi, GroupByKind, para);
    }

    if(true)
    {
        test_vector_perfs( boxes, Phi, GroupByKind, para);
        test_matrix_perfs( boxes, Phi, GroupByKind,para);
    }
    
}
YOCTO_UNIT_TEST_DONE()

