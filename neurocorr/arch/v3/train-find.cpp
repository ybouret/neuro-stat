#include "train.hpp"


size_t Train:: findIndicesWithin(const Unit tauStart,
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


    const size_t   num  = size();
    const _Train  &self = *this;
    switch (num)
    {
        case 0:
            return 0;

        case 1:
        {
            const Unit tau = self[0];
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
    const Unit tauMin = self[0];
    if(tauFinal<tauMin)
    {
        //everybody at left...
        return 0;
    }

    const size_t top    = num-1;
    const Unit   tauMax = self[top];
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
        while(iStartUp-iStartLo>1)
        {
            const size_t jmid = (iStartLo+iStartUp)>>1;
            const Unit   tmid = self[jmid];
            if(tmid<tauStart)
            {
                iStartLo = jmid;
            }
            else
            {
                iStartUp = jmid;
            }
        }

        assert(1+iStartLo==iStartUp);
        assert(self[iStartLo]<tauStart);
        assert(tauStart<=self[iStartUp]);
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
        while(iFinalUp-iFinalLo>1)
        {
            const size_t jmid = (iFinalLo+iFinalUp)>>1;
            const Unit   tmid = self[jmid];
            if(tauFinal<tmid)
            {
                iFinalUp = jmid;
            }
            else
            {
                iFinalLo = jmid;
            }
        }

        assert(1+iFinalLo==iFinalUp);
        assert(self[iFinalLo]<=tauFinal);
        assert(tauFinal<self[iFinalUp]);
    }

    if(iFinalLo>=iStartUp)
    {
        idxStart = iStartUp;
        return (iFinalLo-iStartUp)+1;;
    }
    else
    {
        return 0;
    }

}


size_t Train:: findIndicesWithin_(const Unit tauStart,
                                  const Unit tauFinal,
                                  size_t    &idxStart) const throw()
{
    if(tauStart>=tauFinal)
        return 0;

    const size_t    num = size();
    register size_t i    = 0;
    const _Train   &self = *this;

    while(i<num)
    {
        if(self[i]>=tauStart)
        {
            // found
            idxStart = i;
            while(i<num)
            {
                if(self[i]>tauFinal)
                {
                    // passed
                    break;
                }
                ++i;
            }
            return i-idxStart;
        }
        ++i;
    }


    return 0;
}
