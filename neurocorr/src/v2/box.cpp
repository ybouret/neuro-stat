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

void Box:: computeFor(const PHI &Phi) const
{
    if(trial>=Phi.trials)
    {
        throw exception("");
    }
    
    const size_t iT      = trial;
    const size_t neurons = Phi.neurons;
    const PHI::row &PhiT = Phi[iT];
    
    for(size_t iN=0;iN<neurons;++iN)
    {
        const PHI_Functions &phi   = *PhiT[iN];
        const Train         &train = phi.train;
        extractTauFrom(train);
    }

}
