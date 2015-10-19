#include "phi.hpp"

PHI_Functions:: ~PHI_Functions() throw()
{
}


PHI_Functions:: PHI_Functions(const array<Unit> &spikes, const size_t extra) :
CPW_Functions(1+extra,as_capacity),
train(spikes)
{
    const size_t np   = train.size();
    const size_t nmax = np*2;
    for(size_t i=0;i<=extra;++i)
    {
        // prepare some memory
        append<size_t>(nmax);
    }
}

void PHI_Functions:: build(const Unit delta)
{
    assert(size()>0);
    CPW_Functions &self = *this;

    CPW &phi1 = self[1];
    phi1.buildFrom(train,delta);
    Unit         shift = delta;
    const size_t K     = size();
    const size_t n     = phi1.size();
    for(size_t k=2;k<=K;++k,shift+=delta)
    {
        CPW &phi_k = self[k];
        phi_k.foot = phi1.foot;
        phi_k.free();
        for(size_t i=1;i<=n;++i)
        {
            coord tmp(phi1[i]);
            (Unit &)(tmp.tau) += shift;
            phi_k.__push_back(tmp);
        }
    }

}
