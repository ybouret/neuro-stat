#include "cpw.hpp"

Unit CPW:: integrate_(const Unit tauStart, const Unit tauFinal) const
{
    const CPW &self = *this;
    Unit ans=0;
    for(Unit tau=tauStart;tau<=tauFinal;++tau)
    {
        ans += self(tau);
    }
    return ans;
}
