#include "matrix-builder.hpp"

MatrixBuilder:: ~MatrixBuilder() throw()
{

}


MatrixBuilder:: MatrixBuilder(Matrices<Unit> &matrices,
                              const Boxes    &boxes,
                              const PHI      &usrPhi,
                              Crew           *para) :
box(0),
MG(matrices),
Phi(usrPhi),
mgr(Phi.trials),
tasks(boxes.size*Phi.NK)
{
    //__________________________________________________________________________
    //
    // build manager
    //__________________________________________________________________________
    const size_t trials = Phi.trials;
    for(size_t i=0;i<trials;++i)
    {
        mgr.push_back();
    }

    //__________________________________________________________________________
    //
    //record boxes per trials and update the (0,0) term (length of interval)
    //__________________________________________________________________________
    const size_t nb       = boxes.size;
    const size_t neurones = Phi.neurones;
    const size_t K        = Phi.K;

    for(size_t b=0;b<nb;++b)
    {
        box = &boxes[b];
        assert(box->trial<trials);
        mgr[box->trial].append(box);

        assert(box->indx<MG.count);
        MG[box->indx](0,0) += (1+box->tauFinal-box->tauStart);


    }

    //__________________________________________________________________________
    //
    // 'Linear parts'
    //__________________________________________________________________________
    {
        size_t I=0;
        for(size_t i=0;i<neurones;++i)
        {
            for(size_t k=0;k<K;++k)
            {
                ++I; assert(1+K*i+k==I);
                tasks.append<size_t,size_t,size_t>(i,k,I);
            }
        }
    }

    Crew::single_context mono;
    {
        Kernel kSide(this, & MatrixBuilder::computeSide);
        if(para)
        {
            for(size_t b=0;b<nb;++b)
            {
                box = &boxes[b];
                (*para)(kSide);
            }
        }
        else
        {
            for(size_t b=0;b<nb;++b)
            {
                box = &boxes[b];
                kSide(mono);
            }
        }
    }


}


void MatrixBuilder::computeSide(Context &ctx)
{
    assert(box); assert(box->trial<Phi.trials); assert(box->indx<MG.count);
    const size_t j  = box->trial;
    Matrix<Unit> &G = MG[box->indx];
    size_t offset = 0;
    size_t length = tasks.size;
    ctx.split(offset,length);
    const PHI::row &Phi_j = Phi[j];
    const Unit      tauStart = box->tauStart;
    const Unit      tauFinal = box->tauFinal;
    for(size_t t=offset,counting=0;counting<length;++t,++counting)
    {
        const Task  &task = tasks[t];
        const size_t i    = task.i;
        const size_t k    = task.k;
        const Unit   ans  = Phi_j[i][k].integrate_(tauStart,tauFinal);
        G(0,task.I) = ( G(task.I,0) += ans);
    }
}

