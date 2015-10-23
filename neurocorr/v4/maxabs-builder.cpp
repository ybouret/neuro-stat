#include "maxabs-builder.hpp"

MaxAbsBuilder:: ~MaxAbsBuilder() throw()
{
}


MaxAbsBuilder::MaxAbsBuilder(matrices_of<Unit> &usrMuA,
                             const Boxes       &boxes,
                             const PHI         &usrPhi,
                             threading::crew   *team) :
MuA(usrMuA),
Phi(usrPhi),
box(0)
{
    threading::kernel_executor &kExec = team ? *static_cast<threading::kernel_executor*>(team) : Phi.kSeq;

    assert(1==MuA.cols);
    assert(Phi.dim==MuA.rows);

    // initialize for row
    const size_t count = MuA.count;
    for(size_t q=count;q>0;--q)
    {
        MuA[q](1,1) = 1;
    }

    // for each box
    threading::kernel kRun(this, & MaxAbsBuilder::compute);
    for(size_t b=boxes.size();b>0;--b)
    {
        box = &boxes[b];
        if(box->trial>Phi.trials||box->trial<=0) throw exception("box #%u: invalid trials #%u!", unsigned(b), unsigned(box->trial));
        if(box->indx>count||box->indx<=0)        throw exception("box #%u: invalud index  #%u!", unsigned(b), unsigned(box->indx));
        kExec(kRun);
    }

}


void MaxAbsBuilder:: compute(threading::context &ctx)
{
    assert(box);
    assert(box->trial>0); assert(box->trial<=Phi.trials);
    assert(box->indx>0);  assert(box->indx <=MuA.count );

    // fetch the box index dependent matrix
    matrix_of<Unit> &muA = MuA[box->indx];

    // find the portion that this thread will compute
    size_t offset = 0;
    size_t length = Phi.NK;
    ctx.split(offset,length);

    // loop
    const size_t K         = Phi.K;
    const size_t j         = box->trial;
    const Unit   tauStart  = box->tauStart;
    const Unit   tauFinal  = box->tauFinal;
    const _PHI::row &Phi_j = Phi[j];
    for(size_t idx=offset,counting=length,r=offset+2;counting>0;--counting,++idx,++r)
    {
        ldiv_t d = ldiv(idx, K);
        const size_t i = ++d.quot;
        const size_t k = ++d.rem;
        assert(i<=Phi.neurones);
        assert(k<=Phi.K);
        assert((i-1)*K+(k-1) == idx);
        const CPW &phi    = Phi_j[i][k];
        const Unit ans    = phi.maxAbsOn(tauStart,tauFinal);
        Unit      &target = muA(r,1);
        if(ans>target)  target = ans;
    }

}
