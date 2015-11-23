#include "../vector-builder.hpp"
#include "../matrix-builder.hpp"
#include "../minimiser.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/code/rand.hpp"
#include "yocto/sys/wtime.hpp"
#include "yocto/code/utils.hpp"
#include "yocto/math/core/symdiag.hpp"
#include "yocto/math/core/tao.hpp"

using namespace math;

bool check_params( array<Real> &a, array<int> &sig )
{
    bool ans = true;
    
    for(size_t i=a.size();i>0;--i)
    {
        if(a[i]<0)
        {
            if(sig[i]!=-1)
            {
                ans = false;
                --sig[i];
            }
        }
        else
        {
            if(a[i]>0)
            {
                if(sig[i]!=1)
                {
                    ++sig[i];
                }
            }
            else
            {
                if(sig[i]!=0)
                {
                    ans = false;
                    sig[i] = 0;
                }
            }
        }
    }
    return ans;
}


YOCTO_UNIT_TEST_IMPL(min)
{

    threading::crew team(true);

    size_t neurones   = 4;
    size_t trials     = 1;
    size_t max_spikes = 1000;
    size_t pace       = 5;
    size_t extra      = 1;


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
        matrix<Real> P(n,n),Q(n,n);
        
        vector<Real> b(n),v(n),A(n),d(n),dg(n),sdmb(n),a(n);
        vector<int>  sig(n);
        Real       gam=1.1;
        const Real lnp= log(Real(Phi.dim));
        const Real vfac = 2*gam*lnp;
        const Real afac = gam*lnp/3;
        for(size_t m=1;m<=nm;++m)
        {
            const matrix_of<Unit> &UG = G[m];
            for(size_t i=1;i<=n;++i)
            {
                for(size_t j=1;j<=n;++j)
                {
                   GG[i][j] = UG(i,j)/records.scale;
                }
                A[i] = muA[m](i,1);
            }
            std::cerr << "G=" << GG << ";" << std::endl;
            if( !math::symdiag<Real>::build(GG,dg,P) )
            {
                throw exception("Cannot decompose matrix...");
            }
            math::symdiag<Real>::eigsrtA(dg, P);
            std::cerr << "dg=" << dg << ";" << std::endl;
            std::cerr << "P="  << P  << ";" << std::endl;
            for(size_t i=1;i<=n;++i)
            {
                dg[i] = 1.0 / dg[i];
            }
            for(size_t i=1;i<=n;++i)
            {
                for(size_t j=1;j<=n;++j)
                {
                    Real sum = 0;
                    for(size_t k=1;k<=n;++k)
                    {
                        sum += P[i][k] * dg[k] * P[j][k];
                    }
                    Q[i][j] = sum;
                }
            }
            std::cerr << "Q=" << Q << std::endl;
            
            for(size_t i=1;i<=Phi.neurones;++i)
            {
                std::cerr << "Neurone #" << i << std::endl;
                for(size_t j=1;j<=n;++j)
                {
                    b[j] = mu1[m](j,i);
                    v[j] = mu2[m](j,i);
                    d[j] = sqrt(vfac*v[j])+afac*A[i];
                }
            }
            std::cerr << "b=" << b << std::endl;
            std::cerr << "v=" << v << std::endl;
            std::cerr << "d=" << d << std::endl;
            
            for(int iter=1;iter<=10;++iter)
            {
                for(size_t i=n;i>0;--i)
                {
                    sdmb[i] = sig[i] * d[i] - b[i];
                }
                std::cerr << "sig=" << sig << std::endl;
                std::cerr << "sdmb=" << sdmb << std::endl;
                tao::mul(a, Q, sdmb);
                std::cerr << "a=" << a << std::endl;
                if(check_params(a, sig))
                {
                    std::cerr << "sucess !" << std::endl;
                    break;
                }
                
            }
            

        }
        
#if 0
        Minimiser    opt(usrG);
        for(size_t m=1;m<=nm;++m)
        {
            const matrix_of<Unit> &UG = G[m];
            for(size_t i=1;i<=n;++i)
            {
                for(size_t j=1;j<=n;++j)
                {
                    usrG[i][j] = UG(i,j)/records.scale;
                }
                usrA[i] = muA[m](i,1);
            }
            std::cerr << "usrG=" << usrG << std::endl;
            for(size_t i=1;i<=Phi.neurones;++i)
            {
                std::cerr << "Neurone #" << i << std::endl;
                for(size_t j=1;j<=n;++j)
                {
                    usrB[j] = mu1[m](j,i);
                    usrV[j] = mu2[m](j,i);
                }
                opt.find(1.1, usrB, usrV, usrA);

            }
        }
#endif
        
        

    }


}
YOCTO_UNIT_TEST_DONE()
