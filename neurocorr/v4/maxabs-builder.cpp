#include "maxabs-builder.hpp"

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
    assert(box->indx>0);  assert(box->indx<MuA.count);
    matrix_of<Unit> &muA = MuA[box->indx];
    
}
