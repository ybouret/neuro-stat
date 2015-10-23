#include "vector-builder.hpp"

VectorBuilder:: ~VectorBuilder() throw()
{
}


#include "locate.hpp"

VectorBuilder:: VectorBuilder(matrices_of<Unit> &usrMu1,
                              matrices_of<Unit> &usrMu2,
                              const Boxes       &boxes,
                              const PHI         &usrPhi,
                              threading::crew  *team) :
Mu1(usrMu1),
Mu2(usrMu2),
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

    const size_t neurones = Phi.neurones;
    const size_t count    = Mu1.count;
    const size_t nb       = boxes.size();

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

        // pre-compute the count/start values for each concerned trial
        ins.free();
        for(size_t i=1;i<=neurones;++i)
        {
            const PHI_Functions &Phi_ji = Phi_j[i]; assert(Phi_ji.train);
            const UArray        &tau    = *Phi_ji.train;
            tmp.arr   = &tau;
            tmp.count = Locate::IndicesWithin(tau,tauStart,tauFinal,tmp.start);
            ins.push_back(tmp);

            // and update the first row of each matrices...
            Mu2[m](1,i)  = (Mu1[m](1,i) += tmp.count );
        }

        //
        kExec(kRun);
    }


}

void VectorBuilder:: compute( threading::context &ctx )
{
    assert(box);
    const size_t           j      = box->trial;
    const size_t           m      = box->indx;
    matrix_of<Unit>       &mu1    = Mu1[m];
    matrix_of<Unit>       &mu2    = Mu2[m];
    const size_t           K      = Phi.K;
    const _PHI::row       &Phi_j  = Phi[j];
    const size_t           neurones = Phi.neurones;

    assert(neurones==ins.size());

    // compute work to do
    size_t                 offset = 0;
    size_t                 length = Phi.NK;
    ctx.split(offset,length);

    Moments          moments;
    for(size_t idx=offset,counting=length,r=offset+2;counting>0;--counting,++idx,++r)
    {
        //______________________________________________________________________
        //
        // get the i,k coordinates
        //______________________________________________________________________
        ldiv_t       d = ldiv(idx, K);
        const size_t l = ++d.quot;
        const size_t k = ++d.rem;
        assert(l<=Phi.neurones);
        assert(k<=Phi.K);
        assert((l-1)*K+(k-1) == idx);


        //______________________________________________________________________
        //
        // get data for j,l, and the correspond spike trains
        //______________________________________________________________________
        const PHI_Functions &Phi_jl = Phi_j[l];
        const CPW           &phi    = Phi_jl[k];
        for(size_t i=neurones;i>0;--i)
        {
            const inside info = ins[i]; assert(info.arr);
            phi.evalMoments(*info.arr, info.start, info.count, moments);
            mu1(r,i) += moments.mu1;
            mu2(r,i) += moments.mu2;
        }

    }

    
}