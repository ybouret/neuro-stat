#include "boxes-linear.hpp"

LinearEvaluator:: ~LinearEvaluator() throw()
{
}

LinearEvaluator:: LinearEvaluator(const Boxes &boxes,
                                  const PHI   &UsrPhi,
                                  Matrices    &Bmatrices,
                                  Crew        *para) :
box(0),
Phi(UsrPhi),
B(Bmatrices),
TauList(para?para->size:1),
count(TauList.capacity)
{


    // prepare memory, one list per neurone
    for(size_t i=0;i<count;++i)
    {
        TauList.push_back().ensure(Phi.maxCount);
    }

    Kernel run(this, &LinearEvaluator::compute);
    Crew::single_context mono;
    const size_t nb = boxes.size;
    for(size_t i=0;i<nb;++i)
    {
        box = &boxes[i];
        if(para)
        {
            (*para)(run);
        }
        else
        {
            run(mono);
        }
    }
}


void LinearEvaluator::compute(Context &ctx)
{
    assert(ctx.rank<TauList.size);
    assert(box!=NULL);
    assert(box->trial<Phi.trials);
    assert(box->indx <B.size);
    assert(B[box->indx]);

    //__________________________________________________________________________
    //
    // Local Data
    //__________________________________________________________________________
    UList           &Tau   = TauList[ctx.rank];
    const size_t     N     = Phi.neurones;
    const size_t     K     = Phi.K;
    size_t           offset = 0;
    size_t           length = N;
    Matrix<Unit>    &BB     = *B[box->indx];    //!< target B matrix
    const PHI::row  &Phi_j  = Phi[box->trial];
    ctx.split(offset,length);

    //__________________________________________________________________________
    //
    // Loop over local neurones
    //__________________________________________________________________________
    for(size_t i=offset,ii=0;ii<length;++i,++ii)
    {
        const PHI_Functions &phi_j_i = *Phi_j[i];
        const Train         &train   =  phi_j_i.train;
        box->extract(Tau,train);
        BB(0,i) += Tau.size();

        //______________________________________________________________________
        //
        // Loop over other neurones
        //______________________________________________________________________
        size_t jndx = 0;
        for(size_t l=0;l<N;++l)
        {
            const PHI_Functions &phi_j_l = *Phi_j[l];
            //__________________________________________________________________
            //
            // Loop over Phi functions
            //__________________________________________________________________
            for(size_t k=0;k<K;++k)
            {
                const CPW_Function &phi_j_l_k = phi_j_l[k];
                const Unit ans = phi_j_l_k.sumValuesAtOrdered(Tau);
                BB(++jndx,i) += ans;
            }
        }

    }

}
