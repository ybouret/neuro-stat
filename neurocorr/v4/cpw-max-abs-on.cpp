#include "cpw.hpp"

Unit CPW:: maxAbsOn_(const Unit tauStart, const Unit tauFinal) const throw()
{
    if(tauStart>=tauFinal)
    {
        return 0;
    }
    else
    {
        Unit ans = 0;
        for(Unit tau=tauStart+1;tau<=tauFinal;++tau)
        {
            ans = max_of(ans, UnitAbs( (*this)(tau) ));
        }
        return ans;
    }

}

Unit CPW:: maxAbsOn(const Unit tauStart, const Unit tauFinal) const throw()
{
    if(tauStart>=tauFinal)
    {
        return 0;
    }
    else
    {
        const size_t n = size();
        switch(n)
        {
            case 0: return UnitAbs(foot);
            case 1:
            {
                const coord C  = front();
                const Unit  tt = C.tau;
                if(tauFinal <= tt )
                {
                    return UnitAbs(foot);
                }
                else
                {
                    if(tauStart>=tt)
                    {
                        return UnitAbs(C.value);
                    }
                    else
                    {
                        return max_of(UnitAbs(foot),UnitAbs(C.value));
                    }
                }
            }
        }
        
        return 0;
    }
}
