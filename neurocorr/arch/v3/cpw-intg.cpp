#include "cpw.hpp"

Unit CPW:: integrate_(Unit tauStart, Unit tauFinal) const
{
    Unit ans = 0;
    for(Unit tau=tauStart;tau<tauFinal;++tau)
    {
        ans += (*this)(tau+1);
    }
    return ans;
}

static inline
void __bracket(const Unit  tau,
               const _CPW &self,
               size_t     &jlo,
               size_t     &jup) throw()
{
    assert(jlo<jup);
    assert(jup<self.size);
    while(jup-jlo>1)
    {
        const size_t jmid = (jlo+jup)>>1;
        const Unit   tmid = self[jmid].tau;
        if(tau<=tmid)
        {
            jup = jmid;
        }
        else
        {
            jlo = jmid;
        }
    }
    assert(tau>self[jlo].tau);
    assert(tau<=self[jup].tau);

}


Unit CPW:: integrate(Unit tauStart, Unit tauFinal) const
{

    //__________________________________________________________________________
    //
    //
    // Get rid of trivial cases
    //
    //__________________________________________________________________________
    if(tauFinal<=tauStart)
    {
        return 0;
    }

    const size_t N = size;
    switch(N)
    {
        case 0:
            //__________________________________________________________________
            //
            // constant function
            //__________________________________________________________________
            return (tauFinal-tauStart) * foot;

        case 1:
        {
            //__________________________________________________________________
            //
            // one point somewhere (or not)
            //__________________________________________________________________
            const coord &C   = (*this)[0];
            const Unit   tau = C.tau;
            if(tauFinal<=tau)
            {
                // everybody at left
                return (tauFinal-tauStart)*foot;
            }
            else
            {
                const Unit tail = C.value;
                if(tauStart>=tau)
                {
                    //everybody at right
                    return (tauFinal-tauStart)*tail;
                }
                else
                {
                    //somewhere in between
                    assert(tauStart<=tau);
                    assert(tau<=tauFinal);
                    return (tau-tauStart) * foot + (tauFinal-tau)*tail;

                }
            }
        }
            // never get here;
            assert(die("never get here"));

        default:
            break;
    }

    assert(N>=2);
    const _CPW &self = *this;

    //__________________________________________________________________________
    //
    //
    // still getting rid of trivial cases
    //
    //__________________________________________________________________________
    const coord &cBot = self[0];
    const Unit   tBot = cBot.tau;

    if(tauFinal<=tBot)
    {
        // everybody at left
        return (tauFinal-tauStart) * foot;
    }

    const size_t  top = N-1;
    const coord &cTop = self[top];
    const Unit   tTop = cTop.tau;
    const Unit   vTop = cTop.value;

    if(tauStart>=tTop)
    {
        //everybody at right
        return (tauFinal-tauStart) * vTop;
    }

    //__________________________________________________________________________
    //
    //
    // generic case
    //
    //__________________________________________________________________________
    Unit        ans  = 0;

    //__________________________________________________________________________
    //
    // find index of  tau[iStarLo] < tauStart <= tau[iStartUp]
    //__________________________________________________________________________
    size_t iStartUp = 0;
    size_t iStartLo = 0;
    if(tauStart>tBot)
    {
        iStartUp = top;
        __bracket(tauStart,self,iStartLo,iStartUp);
    }
    else
    {
        ans += (tBot-tauStart) * foot;
        tauStart = tBot;
    }

    //__________________________________________________________________________
    //
    // find index of tau[iFinalLo] < tauFinal <= tau[iFinalUp]
    //__________________________________________________________________________
    size_t iFinalLo = top;
    if(tauFinal<tTop)
    {
        size_t iFinalUp = top;
        iFinalLo        = iStartLo;
        __bracket(tauFinal,self,iFinalLo, iFinalUp);
    }
    else
    {
        ans += (tauFinal-tTop) * vTop;
        tauFinal=tTop;
    }

    assert(iFinalLo>=iStartLo);

    //__________________________________________________________________________
    //
    // find index of partition <= tauFinal
    //__________________________________________________________________________
    if(iFinalLo<iStartUp)
    {
        // both values in the same partition !!
        ans += (tauFinal-tauStart) * self[iFinalLo].value;
    }
    else
    {
        ans += (self[iStartUp].tau - tauStart) * self[iStartLo].value;

        for(size_t i=iStartUp;i<iFinalLo;)
        {
            const coord &C = self[i];
            ++i;
            ans += C.value * (self[i].tau-C.tau);
        }

        ans += (tauFinal-self[iFinalLo].tau)   * self[iFinalLo].value;
    }
    
    
    
    
    return ans;
}
