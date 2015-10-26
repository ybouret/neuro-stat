#include "matrix-builder.hpp"

MatrixBuilder:: ~MatrixBuilder() throw()
{
}

MatrixBuilder:: MatrixBuilder(matrices_of<Unit> &usrMG,
                              const Boxes       &boxes,
                              const PHI         &usrPhi,
                              threading::crew  *team) :
MG(usrMG),
Phi(usrPhi),
mgr(Phi.trials)
{
    assert(MG.rows==Phi.dim);
    assert(MG.cols==Phi.dim);
    assert(Phi.trials==mgr.size());
    const size_t count  = MG.count;
    const size_t trials = Phi.trials;
    const size_t nb     = boxes.size();


    threading::kernel_executor &kExec = team ? *static_cast<threading::kernel_executor*>(team) : Phi.kSeq;
    {
        //______________________________________________________________________
        //
        // Loop on boxes to compute sides terms.
        // Register boxes in manager.
        //______________________________________________________________________
        threading::kernel kSide(this,& MatrixBuilder::computeSide);
        for(size_t b=nb;b>0;--b)
        {
            box            = &boxes[b];
            const size_t j = box->trial;
            const size_t m = box->indx;

            if(j<=0||j>trials) throw exception("MatrixBuilder: invalid box #%u trial=%u!", unsigned(b), unsigned(j) );
            if(m<=0||m>count)  throw exception("MatrixBuilder: invalid box #%u index=%u!", unsigned(b), unsigned(m) );

            matrix_of<Unit> &G = MG[m];
            const Unit tauStart = box->tauStart;
            const Unit tauFinal = box->tauFinal;
            if(tauStart>tauFinal)
                throw exception("MatrixBuilder: invalid box #%u times!",unsigned(b));
            G(1,1) += tauFinal-tauStart;
            kExec(kSide);
            mgr[j].append(box);
        }
    }


    {

    }

}


void MatrixBuilder:: computeSide( threading::context &ctx )
{
    assert(box);
    const size_t j = box->trial;
    const size_t m = box->indx;

    size_t offset = 0;
    size_t length = Phi.NK;
    ctx.split(offset,length);

    const PHI::row  &Phi_j    = Phi[j];
    matrix_of<Unit> &G        = MG[m];
    const size_t     K        = Phi.K;
    const Unit       tauStart = box->tauStart;
    const Unit       tauFinal = box->tauFinal;

    for(size_t idx=offset,r=2,counting=length;counting>0;++idx,++r,--counting)
    {
        //______________________________________________________________________
        //
        // get the i,k coordinates corresponding => row/line
        //______________________________________________________________________
        ldiv_t       d = ldiv(idx,K);
        const size_t i = ++d.quot;
        const size_t k = ++d.rem;
        assert(i<=Phi.neurones);
        assert(k<=Phi.K);
        assert((i-1)*K+(k-1) == idx);
        const CPW &phi  = Phi_j[i][k];
        const Unit intg = phi.integrate_(tauStart,tauFinal);
        G(r,1) = ( G(1,r) += intg );
    }
    
    
}

