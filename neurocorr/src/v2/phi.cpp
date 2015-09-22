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
    self.append<Real,size_t>(scale,0);
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
