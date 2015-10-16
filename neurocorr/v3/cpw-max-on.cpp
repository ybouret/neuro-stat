#include "cpw.hpp"

Unit CPW:: maxAbsOn_(Unit tauStart, Unit tauFinal) const
{
    const CPW &self = *this;
    Unit ans = 0;
    for(Unit tau=tauStart;tau<=tauFinal;++tau)
    {
        const Unit t = UnitAbs( self(tau) );
        if(t>ans)  ans=t;
    }
    return ans;
}
