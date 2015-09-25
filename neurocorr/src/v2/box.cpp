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


void Box:: extractTauFrom( const Train &train ) const
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

void Box:: computeFor(const PHI &Phi, vector<Unit> &b) const
{
    if(trial>=Phi.trials)
    {
        throw exception("Box trial is invalid!!!");
    }
    
    const size_t iT      = trial;
    const size_t neurons = Phi.neurons;
    const PHI::row &PhiT = Phi[iT];
    
    b.free();
    for(size_t iN=0;iN<neurons;++iN)
    {
        const PHI_Functions &phi   = *PhiT[iN];
        const Train         &train = phi.train;
        extractTauFrom(train);
        const size_t N = Tau.size();
        b.push_back(N);
        const size_t K = phi.size;
        for(size_t k=0;k<K;++k)
        {
            const Real phi_trial_neur_k = phi[k]._sumValuesAtOrdered(Tau);
            b.push_back(phi_trial_neur_k);
        }
    }

}
