#include "vector-builder.hpp"

VectorBuilder:: ~VectorBuilder() throw()
{
}


#include "locate.hpp"
#include <cstdlib>

VectorBuilder:: VectorBuilder(matrices_of<Unit> &usrMu1,
                              matrices_of<Unit> &usrMu2,
                              matrices_of<Unit> &usrMuA,
                              const Boxes       &boxes,
                              const PHI         &usrPhi,
                              threading::crew  *team) :
Mu1(usrMu1),
Mu2(usrMu2),
MuA(usrMuA),
Phi(usrPhi),
ins(Phi.neurones,as_capacity),
box(0)
{
    threading::kernel_executor &kExec = team ? *static_cast<threading::kernel_executor*>(team) : Phi.kSeq;
    threading::kernel             kRun(this, &VectorBuilder::compute);
    assert(Mu1.cols==Phi.neurones);
    assert(Mu2.cols==Phi.neurones);
    assert(Mu1.rows==Phi.dim);
    assert(Mu2.rows==Phi.dim);
    assert(Mu1.count == Mu2.count );
    assert(1==MuA.cols);
    assert(Mu1.count==MuA.count);
    assert(MuA.rows==Phi.dim);

    const size_t neurones = Phi.neurones;
    const size_t count    = Mu1.count;
    const size_t nb       = boxes.size();

    for(size_t m=count;m>0;--m)
    {
        MuA[m](1,1) = 1;
    }

    inside tmp;
    for(size_t b=nb;b>0;--b)
    {
        box            = &boxes[b];
        const size_t j = box->trial;
        const size_t m = box->indx;
        if(j>Phi.trials||j<=0) throw exception("box #%u: invalid trials #%u!", unsigned(b), unsigned(box->trial));
        if(m>count||m<=0)      throw exception("box #%u: invalid index  #%u!", unsigned(b), unsigned(box->indx));

        const _PHI::row &Phi_j    = Phi[j];
        const Unit       tauStart = box->tauStart;
        const Unit       tauFinal = box->tauFinal;

        //______________________________________________________________________
        //
        // pre-compute the count/start values for each concerned trial
        //______________________________________________________________________
        ins.free();
        for(size_t i=1;i<=neurones;++i)
        {
            const PHI_Functions &Phi_ji = Phi_j[i]; assert(Phi_ji.train);

            tmp.train = Phi_ji.train;
            tmp.count = Locate::IndicesWithin(*tmp.train,tauStart,tauFinal,tmp.start);
            ins.push_back(tmp);

            // and update the first row of each matrices...
            Mu2[m](1,i)  = (Mu1[m](1,i) += tmp.count );
        }
        assert(neurones==ins.size());

        //______________________________________________________________________
        //
        // fill all other Phi.NK terms
        //______________________________________________________________________
        kExec(kRun);
    }


}

void VectorBuilder:: compute( threading::context &ctx )
{
    assert(box);
    const size_t           j        = box->trial;
    const size_t           m        = box->indx;
    matrix_of<Unit>       &mu1      = Mu1[m];
    matrix_of<Unit>       &mu2      = Mu2[m];
    matrix_of<Unit>       &muA      = MuA[m];
    const size_t           K        = Phi.K;
    const _PHI::row       &Phi_j    = Phi[j];
    const size_t           neurones = Phi.neurones;


    //__________________________________________________________________________
    //
    // compute work to do
    //__________________________________________________________________________
    size_t                 offset = 0;
    size_t                 length = Phi.NK;
    ctx.split(offset,length);

    Moments          moments;
    const Unit       tauStart = box->tauStart;
    const Unit       tauFinal = box->tauFinal;
    for(size_t r=offset+2;length>0;--length,++offset,++r)
    {
        //______________________________________________________________________
        //
        // get the l,k coordinates
        //______________________________________________________________________
        ldiv_t       d = ldiv(offset,K);
        const size_t l = ++d.quot;
        const size_t k = ++d.rem;
        assert(l<=Phi.neurones);
        assert(k<=Phi.K);
        assert((l-1)*K+(k-1) == offset);


        //______________________________________________________________________
        //
        // get data for j,l,k
        //______________________________________________________________________
        const CPW           &phi    = Phi_j[l][k];

#define COMPUTE_MU12(I)                                       \
const inside info = ins[I]; assert(info.train);               \
phi.evalMoments(*info.train, info.start, info.count, moments);\
mu1(r,I) += moments.mu1;                                      \
mu2(r,I) += moments.mu2

        YOCTO_LOOP_FUNC_(neurones,COMPUTE_MU12,1);


        //______________________________________________________________________
        //
        // and the j,l,k term for muA
        //______________________________________________________________________
        const Unit ans    = phi.maxAbsOn(tauStart,tauFinal);
        Unit      &target = muA(r,1);
        if(ans>target)  target = ans;
    }

    
}
