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

Unit CPW:: evalSumOn(const Train &tr,
                     const size_t length,
                     const size_t offset) const throw()
{
    
    
    if(length<=0)
        return 0;
    
    assert(offset<tr.size());
    assert(offset+length-1<tr.size());
    
    const Unit  *Tau = &tr[offset];
    const size_t N   = size;
    const CPW   &self = *this;
    switch(N)
    {
        case 0:
            return length*foot;
            
        case 1:
        {
            const coord &C   = self[0];
            const Unit   tau = C.tau;
            
            size_t       ii  = 0;
            while(ii<length)
            {
                if(Tau[ii]>tau)
                    break;
                ++ii;
            }
            const Unit Vprev = foot;
            const Unit Vnext = C.value;
            const Unit Nprev = ii;
            const Unit Nnext = length-ii;
            return Nprev*Vprev + Nnext * Vnext;
        }
            
        default:
            break;
    }
    
    assert(N>=2);
    
    
    return 0;
    
}

Unit CPW:: evalSumOn_(const Train &tr,
                      const size_t length,
                      const size_t offset) const throw()
{
#ifndef NDEBUG
    if(length>0)
    {
        assert(offset<tr.size());
        assert(offset+length-1<tr.size());
    }
#endif
    
    const CPW &self = *this;
    Unit ans = 0;
    for(size_t i=0,k=offset;i<length;++i,++k)
    {
        const Unit tmp = self(tr[k]);
        ans += tmp;
    }
    return ans;
}


