#include "box.hpp"


Box:: ~Box() throw()
{
}

Box:: Box(const size_t indx,
          const Unit   ini,
          const Unit   end) throw() :
trial(indx),
tauStart(ini),
tauFinal(end),
kind(0)
{
    assert(tauFinal>=tauStart);
}


Box:: Box(const Box &b) throw() :
trial(b.trial),
tauStart(b.tauStart),
tauFinal(b.tauFinal),
kind(b.kind)
{

}


void Box:: extract( UList &Tau, const Train &train ) const
{
    Tau.free();
    const size_t ns = train.size();
    size_t i=0;

    while(i<ns)
    {
        if(train[i]>=tauStart) break;
        ++i;
    }

    while(i<ns)
    {
        const Unit tt = train[i];
        if(tt>tauFinal) break;
        Tau.push_back(tt);
        ++i;
    }

}

#include "yocto/exception.hpp"

void Box:: computeRHS(const PHI &Phi, Matrix<Unit> &B, UList &Tau) const
{
    if(trial>=Phi.trials)
    {
        throw exception("computeVec: Box trial is invalid!!!");
    }

    assert(B.rows==1+Phi.K*Phi.neurones);
    assert(B.cols==Phi.neurones);
    const size_t    iT       = trial;
    const size_t    neurones = Phi.neurones;
    const PHI::row &PhiT     = Phi[iT];
    const size_t    K        = Phi.K;

    for(size_t iN=0;iN<neurones;++iN)
    {
        const PHI_Functions &phi   = *PhiT[iN];
        const Train         &train = phi.train;
        size_t               indx  = 0;

        extract(Tau,train);
        B(0,iN) = Tau.size();
        for(size_t l=0;l<neurones;++l)
        {
            const CPW_Functions &phi = *PhiT[l];
            for(size_t k=0;k<K;++k)
            {
                ++indx;
                B(indx,iN) = phi[k].sumValuesAtOrdered(Tau);
            }
        }
    }

}


void Box:: computeMATRIX(const PHI &Phi, Matrix<Unit> &G) const
{
    assert(G.cols == G.rows );
    assert(G.rows == 1+Phi.K*Phi.neurones);

    if(trial>=Phi.trials)
    {
        throw exception("computeVec: Box trial is invalid!!!");
    }

    G(0,0) = tauFinal-tauStart+1;

    const size_t    iT       = trial;
    const size_t    neurones = Phi.neurones;
    const PHI::row &PhiT     = Phi[iT];
    const size_t    K        = Phi.K;

    size_t indx = 0;
    for(size_t iN=0;iN<neurones;++iN)
    {
        const PHI_Functions &phi_j_i = *PhiT[iN];
        for(size_t k=0;k<K;++k)
        {
            const CPW_Function &phi_j_i_k = phi_j_i[k];
            ++indx;
            G(0,indx) = G(indx,0) = phi_j_i_k.integrate(tauStart,tauFinal);
        }
    }

}
