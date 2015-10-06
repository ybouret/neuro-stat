#include "matrix-builder.hpp"


MatrixBuilder:: ~MatrixBuilder() throw()
{

}


MatrixBuilder:: MatrixBuilder(Matrices<Unit> &matrices,
                              const Boxes    &boxes,
                              const PHI      &usrPhi,
                              Crew           *para) :
box(0),
J(0),
MG(matrices),
Phi(usrPhi),
mgr(Phi.trials),
tasks(boxes.size*Phi.NK)
{
    KernelExecutor    &kExec = *(para ? ((KernelExecutor *)para) : ((KernelExecutor *)&Phi.seq));

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
        assert(box->indx<MG.count);

        mgr[box->trial].append(box);
        MG[box->indx](0,0) += (box->tauFinal-box->tauStart);
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

    {
        Kernel kSide(this, & MatrixBuilder::computeSide);
        for(size_t b=0;b<nb;++b)
        {
            box = &boxes[b];
            kExec(kSide);
        }

    }

    //__________________________________________________________________________
    //
    // Mixed Part
    //__________________________________________________________________________
    {
        Kernel kCore(this, & MatrixBuilder::computeCore);
        for(J=0;J<trials;++J)
        {
            if(mgr[J].size>0)
            {
                kExec(kCore);
            }
        }
    }

}


void MatrixBuilder::computeSide(Context &ctx)
{
    assert(box); assert(box->trial<Phi.trials); assert(box->indx<MG.count);
    const size_t j       = box->trial;
    Matrix<Unit> &G      = MG[box->indx];
    size_t        offset = 0;
    size_t        length = tasks.size;
    ctx.split(offset,length);

    const PHI::row &Phi_j = Phi[j];
    const Unit      tauStart = box->tauStart;
    const Unit      tauFinal = box->tauFinal;
    for(size_t t=offset,counting=0;counting<length;++t,++counting)
    {
        const Task  &task = tasks[t];
        const size_t i    = task.i;
        const size_t k    = task.k;
        const Unit   ans  = Phi_j[i][k].integrate(tauStart,tauFinal);
        G(0,task.I) = ( G(task.I,0) += ans);
    }
}


void MatrixBuilder:: computeCore(Context &ctx)
{
    assert(J<Phi.trials);
    const PHI::row &PHI_J = Phi[J];
    const Mixed    &mixed = Phi.mixed;
    size_t offset = 0;
    size_t length = mixed.size;
    ctx.split(offset,length);
    CPW F(Phi.maxCount*2);
    for(size_t t=offset,counting=0;counting<length;++t,++counting)
    {
        const Mix mix( mixed[t] );
        const CPW &lhs = PHI_J[mix.i][mix.k];
        const CPW &rhs = PHI_J[mix.l][mix.m];
        F.productOf(lhs, rhs);
        for(const BoxNode *node = mgr[J].head; node; node=node->next)
        {
            const Box    *bb  = node->addr; assert(bb->trial==J); assert(bb->indx<MG.count);
            const Unit    ans = F.integrate(bb->tauStart,bb->tauFinal);
            Matrix<Unit> &G   = MG[bb->indx];

            G(mix.I_i_k,mix.I_l_m) = ( G(mix.I_l_m,mix.I_i_k) += ans );
        }
    }
    
}


