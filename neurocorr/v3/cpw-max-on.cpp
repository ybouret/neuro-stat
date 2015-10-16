#include "cpw.hpp"

Unit CPW:: maxAbsOn_(Unit tauStart, Unit tauFinal) const
{
    assert(tauFinal>=tauStart);
    const CPW &self = *this;
    Unit ans = 0;
    for(Unit tau=tauStart;tau<=tauFinal;++tau)
    {
        const Unit t = UnitAbs( self(tau) );
        if(t>ans)  ans=t;
    }
    return ans;
}


Unit CPW:: maxAbsOn(Unit tauStart,Unit tauFinal) const
{
    assert(tauFinal>=tauStart);

    const CPW   &self = *this;
    const size_t n    = self.size;
    switch(n)
    {
        case 0: return UnitAbs(foot);
        case 1:
        {
            const coord &C = self[0];
            const Unit   tau = C.tau;
            if(tauFinal<=tau)
            {
                return UnitAbs(foot);
            }
            else
            {
                return UnitAbs(C.value);
            }
        }

        default:
            break;
    }
    Unit ans = 0;
    return ans;
}