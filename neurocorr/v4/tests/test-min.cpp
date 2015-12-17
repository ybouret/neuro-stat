#include "../vector-builder.hpp"
#include "../matrix-builder.hpp"
#include "../minimiser.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/code/rand.hpp"
#include "yocto/sys/wtime.hpp"
#include "yocto/code/utils.hpp"
#include "yocto/math/core/symdiag.hpp"
#include "yocto/math/core/tao.hpp"
#include "yocto/ios/ocstream.hpp"

using namespace math;

YOCTO_UNIT_TEST_IMPL(min)
{

    threading::crew team(true);

    size_t neurones   = 8;
    size_t trials     = 1;
    size_t max_spikes = 1000;
    size_t pace       = 5;
    size_t extra      = 9;


    const size_t num_boxes=1;

    for(size_t iter=0;iter<1;++iter)
    {
        auto_ptr<Records> pRec( Records::GenerateRandom(neurones, trials, max_spikes, pace));
        const Records    &records = *pRec;
        const Unit        tauMin  = records.minTau-10*pace;
        const Unit        tauMax  = records.maxTau+10*pace;
        const Unit        width   = tauMax-tauMin;
        PHI Phi(records,extra);

        records.display();

        // create somes random boxes with alternate kinds
        matrix<Real> rboxes(4,num_boxes);
        const Unit   bw = max_of<Unit>(1,width/Unit(num_boxes));
        for(size_t i=1;i<=num_boxes;++i)
        {
            rboxes[1][i] = 1+alea_lt(trials);
            rboxes[2][i] = tauMin + Unit(i-1)*bw;
            rboxes[3][i] = rboxes[2][i] + bw;
            rboxes[4][i] = 0;
        }
        std::cerr << "BoxEst=" << rboxes << std::endl;
        Boxes boxes(1.0,rboxes);
        std::cerr << "Boxes=" << boxes << std::endl;


        const size_t          nm = boxes.assignIndices(GroupByKind);
        std::cerr << "Allocating groups of " << nm << " matrices" << std::endl;
        std::cerr << "Phi.neurones=" << Phi.neurones << std::endl;
        std::cerr << "Phi.dim     =" << Phi.dim << std::endl;
        UMatrices muA(nm,Phi.dim,1);
        UMatrices mu1(nm,Phi.dim,Phi.neurones);
        UMatrices mu2(nm,Phi.dim,Phi.neurones);
        UMatrices G(nm,Phi.dim,Phi.dim);

        const Unit delta = 20 * pace;
        std::cerr << "Computing Phi" << std::endl;
        Phi.build(delta, &team);

        std::cerr << "Computing Moments..." << std::endl;
        VectorBuilder vbuild(mu1,mu2,muA,boxes,Phi,&team);
        std::cerr << "mu1=" << mu1[1] << std::endl;
        std::cerr << "mu2=" << mu2[1] << std::endl;
        std::cerr << "muA=" << muA[1] << std::endl;

        std::cerr << "Computing Matrices..." << std::endl;
        MatrixBuilder mbuild(G,boxes,Phi,&team);

        std::cerr << "Would Minimize..." << std::endl;
        const size_t n = Phi.dim;


        matrix<Real> GG(n,n);
        matrix<Real> iG(n,n); //!< pseudo inverse
        matrix<Real> V(n,n);  //!< for symdiag
        vector<Real> lam(n);  //!< for symdiag
        matrix<Real> Mu1(n,Phi.neurones);
        matrix<Real> Mu2(n,Phi.neurones);
        matrix<Real> MuA(n,1);
        matrix<Real> a(n,Phi.neurones);
        vector<Real> count(Phi.neurones);
        vector<Real> H(Phi.neurones);
        vector<Real> err(Phi.neurones);

        Minimisers Opt(GG,iG,Mu1,Mu2,MuA,a,count,H,err,1.1,&team);

        for(size_t m=1;m<=nm;++m)
        {

            // prepare matrices
            for(size_t i=1;i<=n;++i)
            {
                for(size_t j=1;j<=n;++j)
                {
                    GG[i][j] = G(m)[i][j]/records.scale;
                }
                MuA[n][1] = muA(m)[n][1];
                for(size_t neurone=1;neurone<=neurones;++neurone)
                {
                    Mu1[i][neurone] = mu1(m)[i][neurone];
                    Mu2[i][neurone] = mu2(m)[i][neurone];
                }
            }

            if( !symdiag<Real>::build(GG, lam, V) )
            {
                throw exception("cannot symetrize G");
            }

            const size_t nker = symdiag<Real>::eiginv(lam);
            if(nker>0)
            {
                std::cerr << "Found #ker=" << nker << std::endl;
            }
            symdiag<Real>::compute(iG,lam,V);

#if 0
            std::cerr << "First Algorithm..." << std::endl;
            Opt.run(&team);

            std::cerr << "a="     << Opt.a     << std::endl;
            std::cerr << "count=" << Opt.count << std::endl;
            std::cerr << "H="     << Opt.H     << std::endl;
#endif

            std::cerr << "Algorithm V2" << std::endl;
            Opt.run_v2(&team);
            std::cerr << "a="     << Opt.a     << std::endl;
            std::cerr << "count=" << Opt.count << std::endl;
            std::cerr << "H="     << Opt.H     << std::endl;
        }

        
        
    }
    
    
}
YOCTO_UNIT_TEST_DONE()
