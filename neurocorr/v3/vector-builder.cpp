#include "vector-builder.hpp"
#include "yocto/exception.hpp"

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
Phi(usrPhi),
box(0)
{
    assert(Mu1.count==Mu2.count);
    assert(Mu1.count==MuA.count);
    assert(Mu1.rows==Mu2.rows);
    assert(Mu1.rows==MuA.rows);
    assert(Mu2.cols==Mu1.cols);
    assert(1==MuA.cols);

    Kernel             run(this,&VectorBuilder::compute);
    Kernel             runA(this,&VectorBuilder::computeA);
    KernelExecutor    &kExec = *(para ? ((KernelExecutor *)para) : ((KernelExecutor *)&Phi.seq));

    const size_t count  = Mu1.count;  //!< number of matrices
    const size_t trials = Phi.trials; //!< number of trials per neurone
    const size_t nb     = boxes.size; //!< number of boxes
    for(size_t b=0;b<nb;++b)
    {
        box = &boxes[b];
        if(box->trial>=trials)
        {
            throw exception("Box[%u]: invalid trial index (%u/%u)",unsigned(b+1),unsigned(box->trial+1),unsigned(trials));
        }
        const size_t I = box->indx;
        if(I>=count)
        {
            throw exception("Box[%u]: invalid matrix index (%u/%u)",unsigned(b+1),unsigned(I+1),unsigned(count));
        }
        kExec(run);
        kExec(runA);
    }

    
    // finalizing
    const size_t neurones = Phi.neurones;
    for(size_t I=0;I<count;++I)
    {
        Matrix<Unit> &m1 = Mu1[I];
        Matrix<Unit> &m2 = Mu2[I];
        Matrix<Unit> &mA = MuA[I];
        mA(0,0)=1;
        for(size_t i=0;i<neurones;++i)
        {
            const Unit nn = m1(0,i);
            m2(0,i) = nn;
        }
    }


}


void VectorBuilder:: compute(Context &ctx)
{
    assert(box!=NULL);              //checked
    assert(box->trial<Phi.trials);  //checked
    assert(box->indx<Mu1.count);    //checked

    const size_t j = box->trial; // the current trial
    const size_t I = box->indx;  // the matrix index in matrices
    const size_t neurones = Phi.neurones;
    size_t       offset   = 0;
    size_t       length   = neurones;
    ctx.split(offset, length);

    const Unit tauStart   = box->tauStart;
    const Unit tauFinal   = box->tauFinal;
    const PHI::row &PHI_j = Phi[j];

    // for all neurones
    Matrix<Unit> &m1 = Mu1[I];
    Matrix<Unit> &m2 = Mu2[I];
    const size_t  K  = Phi.K;
    Moments       moments;
    for(size_t i=offset,counting=0;counting<length;++counting,++i)
    {
        const Train         &train   = PHI_j[i].train;
        size_t               start   = 0;
        const size_t         num     = train.findIndicesWithin(tauStart,tauFinal,start);

        //______________________________________________________________________
        //
        // first row : the counts, only in mu1, will be copied
        //______________________________________________________________________
        m1(0,i) += num;


        //______________________________________________________________________
        //
        // then computation for other neurones
        //______________________________________________________________________
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
            }
        }
    }
    
}

void VectorBuilder:: computeA(Context &ctx)
{
#if 0
    {
        YOCTO_LOCK(ctx.access);
        std::cerr << ctx.size << "." << ctx.rank << ": computeA on " << box->tauStart << "," << box->tauFinal << std::endl;
    }
#endif
    assert(box!=NULL);              //checked
    assert(box->trial<Phi.trials);  //checked
    assert(box->indx<Mu1.count);    //checked

    const size_t    j        = box->trial; // the current trial
    const size_t    I        = box->indx;  // the matrix index in matrices
    size_t          offset   = 0;
    size_t          length   = Phi.NK;
    const size_t    K        = Phi.K;
    const PHI::row &PHI_j    = Phi[j];
    Matrix<Unit>   &mA       = MuA[I];
    const Unit      tauStart = box->tauStart;
    const Unit      tauFinal = box->tauFinal;
    assert(tauFinal>=tauStart);
    ctx.split(offset,length);
#if 0
    {
        YOCTO_LOCK(ctx.access);
        std::cerr << ctx.size << "." << ctx.rank << ": offset=" << offset << ", length=" << length << std::endl;
    }
#endif

    for(size_t q=offset,counting=0;counting<length;++counting)
    {
        const size_t i = q/K; assert(i<Phi.neurones);
        const size_t k = q%K; assert(K*i+k==q);
        ++q;
        const CPW   &PHI_j_i_k = PHI_j[i][k];
        const Unit   maxA = PHI_j_i_k.maxAbsOn_(tauStart,tauFinal);
        const Unit   curr = mA(q,0);
        if(maxA>curr) mA(q,0) = maxA;
    }
}

