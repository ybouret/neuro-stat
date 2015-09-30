#include "boxes.hpp"

LinearEvaluator:: ~LinearEvaluator() throw()
{
}

LinearEvaluator:: LinearEvaluator(const Boxes &boxes,
                                  const PHI   &UsrPhi,
                                  Matrices    &Bmatrices,
                                  Crew        *para) :
Phi(UsrPhi),
B(Bmatrices),
TauList(para?para->size:1),
count(TauList.capacity)
{

    // prepare memory
    for(size_t i=0;i<count;++i)
    {
        TauList.push_back().ensure(Phi.maxCount);
    }

    Kernel kLinearB(this, &LinearEvaluator::compute);
    if(para)
    {
        (*para)(kLinearB);
    }
    else
    {
        Crew::single_context mono;
        kLinearB(mono);
    }

}


void LinearEvaluator::compute(Context &ctx)
{
    UList &Tau = TauList[ctx.rank];
    size_t offset = 0;
    size_t length = Phi.trials;
    ctx.split(offset, length);
    for(size_t j=offset,ii=0;ii<length;++ii,++j)
    {
        
    }

}