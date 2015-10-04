#include "cpw.hpp"

Unit CPW:: operator()(const Unit tau) const throw()
{
    const size_t np   =  size;
    const _CPW  &self = *this;
    switch (np) {
        case 0:
            return foot;
            
        case 1:
        {
            const coord &C = self[0];
            return (tau <= C.tau) ? foot  : C.value;
        }
            
        default:
            break;
    }
    
    assert(np>=2);
    //const size_t top = np-1;
    const coord &Bot = self[0];
    if(tau<=Bot.tau)
    {
        return foot;
    }
    else
    {
        const size_t  top = np-1;
        const coord  &Top = self[top];
        if(tau>Top.tau)
        {
            return Top.value;
        }
        else
        {
            // coordinate is somewhere inside
            size_t jlo = 0;
            size_t jup = top;
            while(jup-jlo>1)
            {
                const size_t jmid = (jlo+jup)>>1;
                const coord &cmid = self[jmid];
                const Unit   tmid = cmid.tau;
                if(tau<=tmid)
                {
                    jup = jmid;
                }
                else
                {
                    jlo = jmid;
                }
            }
            return self[jlo].value;
        }
    }
    
}


static inline Unit UnitAbs(const Unit x)  throw()
{
    return (x < 0) ? -x : x;
}

void CPW:: evalSumOn(const Train &tr,
                     const size_t length,
                     const size_t offset,
                     Moments     &moments) const throw()
{
    
    moments.reset();
    
    //__________________________________________________________________________
    //
    // Get rid of trivial cases
    //__________________________________________________________________________
    if(length<=0)
    {
        return;
    }
    
    assert(offset<tr.size());
    assert(offset+length-1<tr.size());
    
    const Unit  *Tau  = &tr[offset];
    const size_t N    = size;
    const CPW   &self = *this;
    
    //__________________________________________________________________________
    //
    // not that trivial cases
    //__________________________________________________________________________
    switch(N)
    {
            //__________________________________________________________________
            //
            // constant function....
            //__________________________________________________________________
        case 0:
            moments.mu1 = length*foot;
            moments.mu2 = moments.mu1 * foot;
            moments.muA = UnitAbs(foot);
            return;

            //__________________________________________________________________
            //
            // one point
            //__________________________________________________________________
        case 1:
        {
            
            const coord &C   = self[0];
            const Unit   tau = C.tau;

            //__________________________________________________________________
            //
            // locate tau in Tau's
            //__________________________________________________________________
            if(tau<Tau[0])
            {
                // every times are "at right"
                moments.mu1 = length      * C.value;
                moments.mu2 = moments.mu1 * C.value;
                moments.muA = UnitAbs(C.value);
                return;
            }
            else
            {
                const size_t last = length - 1;
                if(Tau[last]<=tau)
                {
                    // every times are "at left"
                    moments.mu1 = foot*length;
                    moments.mu2 = moments.mu1 * foot;
                    moments.muA = UnitAbs(foot);
                    return;
                }
                else
                {
                    // the splitting time is somewhere in between
                    assert(tau>=Tau[0]);
                    assert(tau<Tau[last]);
                    size_t jlo = 0;
                    size_t jup = last;
                    while(jup-jlo>1)
                    {
                        const size_t jmid=(jup+jlo)>>1;
                        const Unit   tmid=Tau[jmid];
                        if(tau<tmid)
                        {
                            jup = jmid;
                        }
                        else
                        {
                            jlo = jmid;
                        }
                    }
                    const Unit   nNext  = length-jup;
                    const Unit   nPrev  = jup;
                    const Unit   vPrev  = foot;
                    const Unit   vNext  = C.value;
                    const Unit   m1Prev = nPrev*vPrev;
                    const Unit   m1Next = nNext*vNext;
                    moments.mu1 = m1Prev+m1Next;
                    moments.mu2 = m1Prev*vPrev+m1Next*vNext;
                    moments.muA = max_of( UnitAbs(vPrev), UnitAbs(vNext) );
                    return ;
                }
            }
        }
            
        default:
            break;
    }
    
    assert(N>=2);
    
    //__________________________________________________________________________
    //
    // find where the first point enters the function
    //__________________________________________________________________________
    const coord &Cbot  = self[0];
    const Unit   tBot  = Cbot.tau;
    Unit   sum1 = 0;
    Unit   sum2 = 0;
    Unit   maxA = 0;
    size_t indx = 0;
    while(indx<length)
    {
        if(Tau[indx]>tBot)
        {
            break;
        }
        ++indx;
    }
    if(indx>0)
    {
        sum1 = indx * foot;
        maxA = UnitAbs(foot);
        sum2 = sum1 * foot;
    }
    
    //__________________________________________________________________________
    //
    // scan within points
    //__________________________________________________________________________
    const coord &Ctop  = self[N-1];
    const Unit   tTop  = Ctop.tau;
    size_t jprev = 0;
    size_t jnext = 1; //! partition upper index
    Unit   vCurr   = Ctop.value;
    Unit   vCurrSq = vCurr*vCurr;
    while(indx<length)
    {
        const Unit tau = Tau[indx];
        if(tau>tTop)
            break;
        assert(tau>tBot);
        assert(tau<=tTop);
        while(! ( (self[jprev].tau<tau) && (tau<=self[jnext].tau) ) )
        {
            ++jprev;
            ++jnext;
            vCurr   = self[jprev].value;
            vCurrSq = vCurr*vCurr;
        }
        sum1 += vCurr;
        sum2 += vCurrSq;
        maxA  = max_of(maxA,vCurr);
        ++indx;
    }
    
    const Unit remaining = length-indx;
    if(remaining>0)
    {
        assert(Tau[indx]>tTop);
        const Unit vBot = Cbot.value;
        const Unit nv   = remaining*vBot;
        sum1 += nv;
        sum2 += nv * vBot;
        maxA = max_of(vBot,maxA);
    }
    
    moments.mu1 = sum1;
    moments.mu2 = sum2;
    moments.muA = maxA;
}

void CPW:: evalSumOn_(const Train &tr,
                      const size_t length,
                      const size_t offset,
                      Moments     &moments) const throw()
{
    moments.reset();
#ifndef NDEBUG
    if(length>0)
    {
        assert(offset<tr.size());
        assert(offset+length-1<tr.size());
    }
#endif
    
    const CPW &self = *this;
    Unit sum1  = 0;
    Unit sum2  = 0;
    Unit maxA  = 0;
    for(size_t i=0,k=offset;i<length;++i,++k)
    {
        const Unit tmp = self(tr[k]);
        sum1 += tmp;
        const Unit a = UnitAbs(tmp);
        sum2 += tmp*tmp;
        if(a>maxA) maxA = a;
    }
    moments.mu1 = sum1;
    moments.mu2 = sum2;
    moments.muA = maxA;
}


