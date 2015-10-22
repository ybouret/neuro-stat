#include "cpw.hpp"

CPW:: CPW() throw() : _CPW(), foot(0)
{
}

CPW:: CPW(size_t n) : _CPW(n,as_capacity), foot(0)
{
}


CPW:: ~CPW() throw()
{
}

CPW:: CPW( const CPW &other ) : _CPW(other), foot(other.foot)
{
}


bool operator==(const CPW &lhs, const CPW &rhs) throw()
{
    if(lhs.size()!=rhs.size()) return false;
    if(lhs.foot!=rhs.foot)     return false;
    for(size_t i=lhs.size();i>0;--i)
    {
        if(lhs[i]!=rhs[i]) return false;
    }
    return true;
}

bool operator!=(const CPW &lhs, const CPW &rhs) throw()
{
    return !(lhs==rhs);
}



void CPW:: add(const Unit tau,const Unit value)
{
    append<Unit,Unit>(tau,value);
    size_t n    = size();
    _CPW  &self = *this;
    while(n>1)
    {
        coord &curr = self[n];
        coord &prev = self[n-1];
        if(curr.tau<prev.tau)
        {
            bswap(curr,prev);
        }
        else
        {
            break;
        }
        --n;
    }
    
}

Unit CPW:: operator()(const Unit tau) const throw()
{
    const size_t n = size();
    switch (n)
    {
        case 0:
            return foot;

        case 1:
        {
            const coord C = front();
            return (tau<=C.tau) ? foot : C.value;
        }

        default:
            break;
    }
    assert(n>=2);


    const coord lo = front();
    if(tau<=lo.tau) return foot;

    const coord hi = back();
    if(tau>hi.tau) return hi.value;

    const _CPW &self = *this;
    size_t jlo = 1;
    size_t jhi = n;

    // find tau[jlo]<tau<=tau[jhi]
    while(jhi-jlo>1)
    {
        const size_t jmid = (jhi+jlo)>>1;
        const Unit   tmid = self[jmid].tau;
        if(tmid<tau)
        {
            jlo = jmid;
        }
        else
        {
            jhi = jmid;
        }
    }

    return self[jlo].value;
}

#include "yocto/ios/ocstream.hpp"

void CPW:: save(const char *filename) const
{
    ios::wcstream fp(filename);
    const size_t n = size();
    switch(n)
    {
        case 0:
            fp("-1 %ld\n",long(foot));
            fp("1 %ld\n",long(foot));
            break;

        case 1:
        {
            const coord &C = front();
            fp("%ld %ld\n", long(C.tau), long(foot) );
            fp("%ld %ld\n", long(C.tau), long(C.value) );
        }
            break;

        default:
        {
            assert(size()>=2);
            const _CPW &self = *this;
            fp("%ld %ld\n",long(self[1].tau), long(foot));
            for(size_t i=1;i<n;++i)
            {
                fp("%ld %ld\n", long(self[i].tau),   long(self[i].value));
                fp("%ld %ld\n", long(self[i+1].tau), long(self[i].value));
            }
            fp("%ld %ld\n", long(self[n].tau), long(self[n].value));
        }
    }


}

void CPW:: save_sample(const char *filename) const
{
    ios::wcstream fp(filename);
    const size_t n = size();

    Unit tauLo = 0;
    Unit tauHi = 0;
    switch(n)
    {
        case 0: tauLo=-1;tauHi=1; break;
        default: tauLo=front().tau-1;tauHi=back().tau+1;break;
    }
    for(Unit tau=tauLo;tau<=tauHi;++tau)
    {
        const Unit v =(*this)(tau);
        fp("%ld %ld\n", long(tau), long(v));
    }
}

