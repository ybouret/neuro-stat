#include "../phi.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/code/rand.hpp"

#include "yocto/ios/ocstream.hpp"
#include "yocto/sys/wtime.hpp"

#include "yocto/string/conv.hpp"
#include "yocto/sys/timings.hpp"

#define YOCTO_TIME_PHI 2.0

YOCTO_UNIT_TEST_IMPL(phi)
{
    Crew para(true);

    size_t trials   = 10;  if( argc>1 ) trials   = strconv::to<size_t>(argv[1],"trials");
    size_t neurones = 4;   if( argc>2 ) neurones = strconv::to<size_t>(argv[2],"neurones");
    size_t spikes   = 100; if( argc>3 ) spikes   = strconv::to<size_t>(argv[3],"spikes");
    size_t extra    = 2;   if( argc>4 ) extra    = strconv::to<size_t>(argv[4],"extra");

    auto_ptr<Records> pRec( Records::CreateRandom(trials,neurones,spikes, 3) );
    Records &records = *pRec;
    records.display();

    std::cerr << "Allocating Memory for Phi" << std::endl;
    PHI Phi(records,extra);
    std::cerr << "Phi.K="   << Phi.K   << std::endl;
    std::cerr << "Phi.dim=" << Phi.dim << std::endl;

    const Unit delta = 5;
    timings tmx;
    YOCTO_TIMINGS(tmx, YOCTO_TIME_PHI,Phi.compute(delta,NULL));
    const double seqSpeed = tmx.speed;
    std::cerr << "seqSpeed=" << seqSpeed << std::endl;
    
    YOCTO_TIMINGS(tmx, YOCTO_TIME_PHI,Phi.compute(delta,&para));
    const double parSpeed = tmx.speed;
    std::cerr << "parSpeed=" << parSpeed << std::endl;

    std::cerr << "speedUp =" << parSpeed/seqSpeed << std::endl;

}
YOCTO_UNIT_TEST_DONE()
