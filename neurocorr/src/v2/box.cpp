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


UList &Box:: buildTauFor( const Train &train ) const throw()
{
    assert(train.Tau.capacity()>=train.size());
    UList &Tau = train.Tau;

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

    return Tau;
}

#include "yocto/exception.hpp"

void Box:: computeFor(const PHI &Phi, Matrix<Unit> &B ) const throw()
{
    if(trial>=Phi.trials)
    {
        throw exception("Box trial is invalid!!!");
    }

    assert(B.rows==Phi.neurones);
    assert(B.cols==1+Phi.K);
    const size_t    iT       = trial;
    const size_t    neurones = Phi.neurones;
    const PHI::row &PhiT     = Phi[iT];
    const size_t    K        = Phi.K;

    for(size_t iN=0;iN<neurones;++iN)
    {
        const PHI_Functions &phi   = *PhiT[iN];
        const Train         &train = phi.train;
        const UList         &Tau = buildTauFor(train);
        const size_t         np = Tau.size();
        B(iN,0) = np;
        for(size_t k=0;k<K;++k)
        {
            const Unit phi_trial_neur_k = phi[k].sumValuesAtOrdered(Tau);
            B(iN,1+k) = phi_trial_neur_k;
        }
    }

}
