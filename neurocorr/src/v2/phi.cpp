#include "phi.hpp"

Phi:: ~Phi() throw()
{
}

Phi:: Phi(const size_t extra,
          const Real   scale,
          const Train &train,
          const Unit   deltaUnit) :
PhiBase(1+extra)
{
    PhiBase &self = *this;

    //create 'mother phi'
    self.append<Real>(scale);
    CPW_Function &phi0 = self[0];
    phi0.buildFrom(train, deltaUnit, true);

    // create shifted functions
    for(size_t i=1;i<=extra;++i)
    {
        const Unit delta = i*deltaUnit;
        self.append<const CPW_Function &,Unit>(phi0,delta);
    }

}


void Phi:: update(const Train &train, const Unit deltaUnit)
{
    assert(size>0);
    PhiBase &self = *this;
    CPW_Function &phi0 = self[0];
    phi0.buildFrom(train, deltaUnit, true);
    for(size_t i=1;i<=size;++i)
    {
        const Unit    delta = i*deltaUnit;
        CPW_Function &phi = self[i];
        phi.copyAndShift(phi0,delta);
    }
}

////////////////////////////////////////////////////////////////////////////////

PhiSet:: ~PhiSet() throw()
{
}

PhiSet:: PhiSet(const size_t extra,
                const Neuron &nn,
                const Unit   deltaUnit ) :
PhiSetBase(nn.trials),
neuron(nn)
{
    for(size_t i=0;i<neuron.trials;++i)
    {
        append<size_t, Real, const Train &, Unit>(extra,
                                                  neuron.scale,
                                                  neuron[i],
                                                  deltaUnit);
    }
}

void PhiSet:: update(const Unit deltaUnit)
{
    assert(neuron.trials==size);
    PhiSetBase &self = *this;
    for(size_t i=0;i<size;++i)
    {
        self[i].update(neuron[i],deltaUnit);
    }
}
