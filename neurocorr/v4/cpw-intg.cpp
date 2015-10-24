#include "cpw.hpp"

Unit CPW:: integrate_(const Unit tauStart, const Unit tauFinal) const throw()
{
    if(tauStart>=tauFinal)
    {
        return 0;
    }
    else
    {
        const CPW &self = *this;
        Unit ans = 0;
        for(Unit tau = tauStart; tau < tauFinal; ++tau )
        {
            ans += self(tau+1);
        }
        return ans;
    }
}
