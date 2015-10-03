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
    
    if(length<=0)
    {
        return;
    }
    
    assert(offset<tr.size());
    assert(offset+length-1<tr.size());
    
    const Unit  *Tau  = &tr[offset];
    const size_t N    = size;
    const CPW   &self = *this;
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
                    assert(tau>=Tau[0]);
                    assert(tau<Tau[last]);

                }
            }

            size_t       ii  = 0;
            while(ii<length)
            {
                if(Tau[ii]>tau)
                    break;
                ++ii;
            }
            const Unit Vprev  = foot;
            const Unit Vnext  = C.value;
            const Unit Nprev  = ii;
            const Unit Nnext  = length-ii;
            const Unit NVprev = Nprev*Vprev;
            const Unit NVnext = Nnext*Vnext;
            moments.mu1 = NVprev + NVnext;
            moments.mu2 = NVprev*Vprev + NVnext*Vnext;
            if(ii<=0)
            {
                moments.muA = UnitAbs(Vnext);
            }
            else
            {
                if(ii>=length)
                {
                    moments.muA = UnitAbs(Vprev);
                }
                else
                {
                    moments.muA = max_of(UnitAbs(Vnext), UnitAbs(Vprev));
                }
            }
            return;
        }
            
        default:
            break;
    }
    
    assert(N>=2);
    
    
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


