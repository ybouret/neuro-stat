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


void Box:: extractFrom( const Train &train, UList &tau ) const
{
    tau.free();
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
        tau.push_back(tt);
        ++i;
    }

}

#if 0
void Box:: build_rhs(const PHI &phi) const
{
    assert(trial<=phi.trials);
    const size_t iTrial  = trial;
    const size_t neurons = phi.neurons;

    for(size_t iN=0;iN<neurons;++iN)
    {
        
    }


}
#endif

