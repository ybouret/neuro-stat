#include "phi.hpp"

PHI_Functions:: ~PHI_Functions() throw()
{
}

PHI_Functions:: PHI_Functions() throw() :
CPW_Functions(),
train(0)
{
}


PHI_Functions:: PHI_Functions(const array<Unit> &spikes, const size_t extra) :
CPW_Functions(1+extra,as_capacity),
train(&spikes)
{
    const size_t np   = train->size();
    const size_t nmax = np*2;
    for(size_t i=0;i<=extra;++i)
    {
        // prepare some memory
        append<size_t>(nmax);
    }
}

void PHI_Functions:: build(const Unit delta, UVector &shift)
{
    assert(size()>0);
    assert(train);
    assert(delta>0);

    CPW_Functions &self = *this;

    CPW &phi1 = self[1];
    phi1.buildFrom(*train,delta,shift);
    Unit         k_delta = delta;
    const size_t K       = size();
    const size_t n       = phi1.size();
    for(size_t k=2;k<=K;++k,k_delta+=delta)
    {
        CPW &phi_k = self[k];
        phi_k.foot = phi1.foot;
        phi_k.free();
        for(size_t i=1;i<=n;++i)
        {
            coord tmp(phi1[i]);
            (Unit &)(tmp.tau) += k_delta;
            phi_k.__push_back(tmp);
        }
    }

}


PHI:: ~PHI() throw()
{
}


static const int __buildFlags = 0; //!< for matrix

PHI:: PHI(const Records &records, const size_t extra) :
_PHI(records,extra,__buildFlags),
K(1+extra),
trials(rows),
neurones(cols),
NK(neurones*K),
dim(1+NK),
maxCount(records.maxCount),
kSeq(),
delta(0),
kBuild(this, &PHI::onBuild),
shifts()
{
}

#include "yocto/sequence/slots.hpp"

void PHI:: build(const Unit deltaUnits, threading::crew *team)
{
    threading::kernel_executor &kExec = team ? *static_cast<threading::kernel_executor*>(team) : kSeq;
    delta = deltaUnits;
    

    const size_t nthr = kExec.num_threads();
    shifts.ensure(nthr);
    while(shifts.size()<nthr)
    {
        const UVecPtr p( new UVector(maxCount) );
        shifts.__push_back(p);
    }
    
    kExec( kBuild );
}

#include "yocto/code/unroll.hpp"

#define NC_BUILD_PHI(I) self.fetch(I).build(delta,shift)

void PHI:: onBuild(threading::context &ctx)
{
    _PHI  &self   = *this;
    size_t offset = 0;
    size_t length = items;
    ctx.split(offset,length);
    assert(shifts.size()>=ctx.size);
    UVector &shift = *shifts[ctx.indx];
    YOCTO_LOOP_FUNC(length,NC_BUILD_PHI, offset);
}
