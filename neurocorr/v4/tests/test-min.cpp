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

    size_t neurones   = 4;
    size_t trials     = 1;
    size_t max_spikes = 1000;
    size_t pace       = 5;
    size_t extra      = 3;


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
        matrix<Real> GG(n);
        matrix<Real> Q(n,n);
        vector<Real> ev(n);
        vector<Real> a1(n);


        Minimiser Opt(GG,1e-6);
        for(size_t m=1;m<=nm;++m)
        {
            const matrix_of<Unit> &UG = G[m];

            for(size_t i=1;i<=n;++i)
            {
                for(size_t j=1;j<=n;++j)
                {
                    GG[i][j] = UG(i,j)/records.scale;
                }
            }
            std::cerr << "G0=" << GG << ";" << std::endl;
            if(!symdiag<Real>::build(GG, ev, Q) )
            {
                throw exception("Cannot Symmetrize G");
            }
            symdiag<Real>::eigsrtA(ev,Q);

            //std::cerr << "G=" << GG << ";" << std::endl;
            //std::cerr << "Q=" << Q   << ";" << std::endl;
            std::cerr << "ev=diag(" << ev << ");" << std::endl;

            for(size_t neurone=1;neurone<=neurones;++neurone)
            {
                Opt.prepare(mu1(m), mu2(m), muA(m), neurone, 1.1);

#if 0
                tao::mul_trn(Opt.y,Q,Opt.b);
                for(size_t i=1;i<=n;++i)
                {
                    Opt.y[i] /= ev[i];
                }
                tao::mul(Opt.a,Q,Opt.y);
                std::cerr << "a0=" << Opt.a << std::endl;
                for(size_t nsink=0;nsink<2;++nsink)
                {
                    std::cerr << "// sink" << std::endl;
                    Opt.sink();

                    std::cerr << "a=" << Opt.a << std::endl;
                }


                continue;

                size_t iter=1;
                for(;;++iter)
                {
                    Opt.update();
                    //std::cerr << "a=" << Opt.a << std::endl;
                    if(Opt.converged())
                        break;
                }
                std::cerr << "iter=" << iter << std::endl;
                std::cerr << "a1=" << Opt.a << std::endl;
                tao::set(a1,Opt.a);
#endif


                size_t count = 1;
                tao::ld(Opt.a,0);
                tao::ld(Opt.s,0);
                std::cerr << "a0=" << Opt.a << std::endl;
                std::cerr << "s0=" << Opt.s << std::endl;
                {
                    Opt.update_v2();
                    std::cerr << "a1=" << Opt.a << std::endl;
                    std::cerr << "s1=" << Opt.s << std::endl;
                    Opt.compute_q();
                    Opt.forward();
                }
                std::cerr << "count=" << count << std::endl;

                std::cerr << std::endl;
            }
            
            
            
            
        }
        
        
        
    }
    
    
}
YOCTO_UNIT_TEST_DONE()
