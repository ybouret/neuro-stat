#include "cpw.hpp"


// find self.tau[jlo]<=tau<=self.tau[jup]
static inline
void __locate(const slots_of<coord> &self,
              const Unit  tau,
              size_t     &jlo,
              size_t     &jup) throw()
{
    assert(jlo<self.size);
    assert(jup<self.size);
    assert(jlo<jup);
    assert(tau>=self[jlo].tau);
    assert(tau<=self[jup].tau);
    while(jup-jlo>1)
    {
        const size_t jmid = (jup+jlo)>>1;
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
}


size_t CPW:: findIndicesWithin(const Unit tauStart,
                               const Unit tauFinal,
                               size_t    &idxStart) const throw()
{

    //__________________________________________________________________________
    //
    //
    // Get rid of trivial cases, level 1
    //
    //__________________________________________________________________________
    if(tauFinal<=tauStart)
    {
        return 0;
    }


    const size_t num  = size;
    const _CPW  &self = *this;
    switch (num)
    {
        case 0:
            return 0;

        case 1:
        {
            const Unit tau = self[0].tau;
            if(tauStart<=tau&&tau<=tauFinal)
            {
                idxStart = 0;
                return 1;
            }
            else
            {
                return 0;
            }
        }

        default:
            break;
    }

    assert(num>=2);
    //__________________________________________________________________________
    //
    //
    // Get rid of trivial cases, level 2
    //
    //__________________________________________________________________________
    const Unit tauMin = self[0].tau;
    if(tauFinal<tauMin)
    {
        //everybody at left...
        return 0;
    }

    const size_t top    = num-1;
    const Unit   tauMax = self[top].tau;
    if(tauStart>tauMax)
    {
        //everybody at right
        return 0;
    }


    //__________________________________________________________________________
    //
    //
    // Find iStartUp, index of coordinate>=tauStart
    //
    //__________________________________________________________________________
    size_t iStartUp = 0;
    size_t iStartLo = 0;
    if(tauStart>tauMin)
    {
        assert(tauStart<=tauMax);
        iStartUp = top;
        __locate(self,tauStart,iStartLo,iStartUp);
        assert(1+iStartLo==iStartUp);
        assert(self[iStartLo].tau<=tauStart);
        assert(tauStart<=self[iStartUp].tau);
    }

    //__________________________________________________________________________
    //
    //
    // Find iFinalLo, index of coordinate<=tauFinal
    //
    //__________________________________________________________________________
    size_t iFinalLo = top;
    if(tauFinal<tauMax)
    {
        assert(tauFinal>=tauMin);
        iFinalLo = iStartLo;
        size_t iFinalUp = top;
        __locate(self,tauFinal,iFinalLo,iFinalUp);
        assert(1+iFinalLo==iFinalUp);
        assert(self[iFinalLo].tau<=tauFinal);
        assert(tauFinal<=self[iFinalUp].tau);
    }

    if(iFinalLo>=iStartUp)
    {
        idxStart = iStartUp;
        return (iFinalLo-iStartUp)+1;
    }
    else
    {
        return 0;
    }
    
}

