#include "vector-builder.hpp"

VectorBuilder:: ~VectorBuilder() throw()
{
}



VectorBuilder:: VectorBuilder(Matrices<Unit> &mu1,
                              Matrices<Unit> &mu2,
                              Matrices<Unit> &muA,
                              const Boxes    &boxes,
                              const PHI      &usrPhi,
                              Crew           *para) :
Mu1(mu1),
Mu2(mu2),
MuA(muA),
Phi(usrPhi)
{
    assert(Mu1.count==Mu2.count);
    assert(Mu1.count==MuA.count);
    Kernel             run(this,&VectorBuilder::compute);
    KernelExecutor    &kExec = *(para ? ((KernelExecutor *)para) : ((KernelExecutor *)&Phi.seq));

    const size_t nb = boxes.size;
    for(size_t b=0;b<nb;++b)
    {
        box = &boxes[b];
        kExec(run);
    }

    
    // finalizing
    const size_t neurones = Phi.neurones;
    const size_t count    = Mu1.count;
    for(size_t I=0;I<count;++I)
    {
        Matrix<Unit> &m1 = Mu1[I];
        Matrix<Unit> &m2 = Mu2[I];
        Matrix<Unit> &mA = MuA[I];
        for(size_t i=0;i<neurones;++i)
        {
            const Unit nn = m1(0,i);
            m2(0,i) = nn*nn;
            mA(0,i) = 1;
        }
    }


}


void VectorBuilder:: compute(Context &ctx)
{
    assert(box!=NULL);
    assert(box->trial<Phi.trials);
    assert(box->indx<Mu1.count);

    const size_t j = box->trial;
    const size_t I = box->indx;
    const size_t neurones = Phi.neurones;
    size_t       offset = 0;
    size_t       length = neurones;
    ctx.split(offset, length);

    const Unit tauStart   = box->tauStart;
    const Unit tauFinal   = box->tauFinal;
    const PHI::row &PHI_j = Phi[j];
    // for all neurones
    Matrix<Unit> &m1 = Mu1[I];
    Matrix<Unit> &m2 = Mu2[I];
    Matrix<Unit> &mA = MuA[I];
    const size_t  K  = Phi.K;
    Moments       moments;
    for(size_t i=offset,counting=0;counting<length;++counting,++i)
    {
        //const CPW_Functions &PHI_j_i =
        const Train         &train   = PHI_j[i].train;
        size_t               start   = 0;
        const size_t         num     = train.findIndicesWithin(tauStart,tauFinal,start);

        // first row : the counts
        m1(0,i) += num;


        // then computation
        size_t r = 0;
        for(size_t l=0;l<neurones;++l)
        {
            const CPW_Functions &PHI_j_l = PHI_j[l];
            for(size_t k=0;k<K;++k)
            {
                const CPW &PHI_j_l_k = PHI_j_l[k];
                PHI_j_l_k.evalSumOn(train,num,start,moments);
                ++r;
                m1(r,i) += moments.mu1;
                m2(r,i) += moments.mu2;
                mA(r,i)  = max_of(mA(r,i),moments.muA);
            }
        }
    }
    
}