#include "boxes-intg.hpp"



IntegralEvaluator:: ~IntegralEvaluator() throw()
{
}




IntegralEvaluator:: IntegralEvaluator(const Boxes    &boxes,
                                      const PHI      &usrPhi,
                                      Matrices       &Gmatrices,
                                      Crew           *para) :
Phi(usrPhi),
G(Gmatrices),
Tasks(boxes.size*Phi.neurones*Phi.K)
{
    const size_t nb = boxes.size;
    const size_t N  = Phi.neurones;
    const size_t K  = Phi.K;
    assert(nb*N*K==Tasks.capacity);
    
    for(size_t b=0;b<nb;++b)
    {
        const Box *box = &boxes[b];
        assert(box->trial<Phi.trials);
        assert(box->indx<G.size);
        Matrix<Unit> &GG = *G[box->indx];
        GG(0,0) += (box->tauFinal-box->tauStart)+1;
        size_t I=0;
        for(size_t i=0;i<N;++i)
        {
            for(size_t k=0;k<K;++k)
            {
                Tasks.append<const Box *,size_t,size_t,size_t>(box,i,k,++I);
            }
        }
    }
    Kernel run(this, &IntegralEvaluator::compute);
    if(para)
    {
        (*para)(run);
    }
    else
    {
        Crew::single_context mono;
        run(mono);
    }
}


void IntegralEvaluator::compute(Context &ctx)
{
    size_t offset = 0;
    size_t length = Tasks.size;
    ctx.split(offset,length);
    for(size_t p=offset,pp=0;pp<length;++p,++pp)
    {
        const Task  &todo = Tasks[p];
        const Box   *box  = todo.box;
        const size_t j    = box->trial;
        const size_t i    = todo.neurone;
        const size_t k    = todo.k;
        const size_t I    = todo.I;
        
        const CPW_Function &F   = (*Phi[j][i])[k];
        const Unit          ans = F.integrate(box->tauStart,box->tauFinal);
        Matrix<Unit>       &GG  = *G[box->indx];
        (void)ans;
        GG(0,I) = ( GG(I,0) += ans);
    }
    
}
