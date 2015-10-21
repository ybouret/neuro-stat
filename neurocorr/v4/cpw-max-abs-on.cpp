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
        //______________________________________________________________________
        //
        // Special Cases
        //______________________________________________________________________
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
                assert(die("never get here"));

            default:
                break;
        }

        //______________________________________________________________________
        //
        // Generic Cases
        //______________________________________________________________________
        assert(n>=2);
        const coord lower = front();
        if(tauFinal<=lower.tau)
        {
            // all at left
            //std::cerr << "|all left|" << std::endl;
            return UnitAbs(foot);
        }
        else
        {
            const coord upper = back();
            if(tauStart>=upper.tau)
            {
                // all at right
                //std::cerr << "|all right|" << std::endl;
                return UnitAbs(upper.value);
            }
            else
            {
                assert(tauFinal>lower.tau);
                assert(tauStart<upper.tau);
                //______________________________________________________________
                //
                // generic case
                //______________________________________________________________
                const CPW &self = *this;
                Unit       ans  = 0;

                // Initialize
                size_t i = 1;
                if(tauStart<lower.tau)
                {
                    ans = UnitAbs(foot);
                }
                else
                {
                    // find values such that tau[jlo]<=tauStart<tau[jhi]
                    size_t  jlo = 1;
                    size_t &jhi = i;
                    jhi         = n;
                    while(jhi-jlo>1)
                    {
                        const size_t jmid = (jlo+jhi)>>1;
                        const Unit   tmid = self[jmid].tau;
                        if(tauStart<tmid)
                        {
                            jhi = jmid;
                        }
                        else
                        {
                            jlo = jmid;
                        }
                    }
                    assert(1==jhi-jlo);
                    assert(self[jlo].tau<=tauStart);
                    assert(tauStart<self[jhi].tau);
                    ans = UnitAbs(self[jlo].value);
                }

                // starting from i until > tauFinal (or not)
                for(;i<=n;++i)
                {
                    const coord curr = self[i];
                    if(curr.tau>=tauFinal)
                        break;
                    const Unit tmp = UnitAbs(curr.value);
                    if(tmp>ans)
                    {
                        ans=tmp;
                    }
                }


                return ans;
            }
        }
        

    }
}
