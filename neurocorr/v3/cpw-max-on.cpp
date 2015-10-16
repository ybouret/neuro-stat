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

    //__________________________________________________________________________
    //
    // Trivial cases, part 1
    //__________________________________________________________________________

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
    assert(n>=2);

    const coord &Cbot = self[0];
    const Unit   tBot = Cbot.tau;
    //__________________________________________________________________________
    //
    // Trivial cases, part 2
    //__________________________________________________________________________

    // everybody is foot ?
    if(tauFinal<=tBot)
    {
        return UnitAbs(foot);
    }

    const size_t ntop = n-1;
    const coord &Ctop = self[ntop];
    const Unit   tTop = Ctop.tau;
    const Unit   vTop = Ctop.value;

    // everybody is last value ?
    if(tauStart>tTop)
    {
        return UnitAbs(vTop);
    }

    //std::cerr << "tauStart=" << tauStart << ", tauFinal=" << tauFinal << std::endl;
    //std::cerr << "tBot    =" << tBot<<      ", tTop    =" << tTop     << std::endl;
    //__________________________________________________________________________
    //
    // Generic Algorithm
    //__________________________________________________________________________
    Unit   ans = 0;
    size_t idx = 0;
    if(tauStart<=tBot)
    {
        //std::cerr << "init foot" << std::endl;
        ans = UnitAbs(Cbot.value);
    }
    else
    {
        assert(tauStart<=tTop);
        size_t ihi = ntop;
        while(ihi-idx>1)
        {
            const size_t im = (idx+ihi)/2;
            const Unit   tm = self[im].tau;
            if(tm<tauStart)
            {
                idx = im;
            }
            else
            {
                ihi = im;
            }
        }
        assert(tauStart>self[idx].tau);
        assert(tauStart<=self[ihi].tau);
        ans = UnitAbs(self[idx].value);
    }

    //std::cerr << "Start Idx=" << idx << std::endl;
    assert(idx<n);
    while(idx<n)
    {
        const coord &C = self[idx];
        if(tauFinal<=C.tau)
        {
            //std::cerr << "\tend after " << C.tau << std::endl;
            return ans;
        }
        ans = max_of(ans,C.value);
        ++idx;
    }

    if(tauFinal>tTop)
    {
        ans = max_of(ans,vTop);
    }
    
    return ans;
}