#include "cpw.hpp"

CPW:: CPW() throw() : _CPW(), foot(0)
{
}

CPW:: ~CPW() throw()
{
}

CPW:: CPW( const CPW &other ) : _CPW(other), foot(other.foot)
{
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
            fp("%ld %ld\n", C.tau, foot );
            fp("%ld %ld\n", C.tau, C.value);
        }
            break;

        default:
        {
            assert(size()>=2);
            const _CPW &self = *this;
            fp("%ld %ld\n",long(self[1].tau), foot);
            for(size_t i=1;i<n;++i)
            {
                fp("%ld %ld\n", long(self[i].tau), long(self[i].value));
                fp("%ld %ld\n", long(self[i+1].tau), long(self[i].value));
            }
            fp("%ld %ld\n", long(self[n].tau), long(self[n].value));
        }
    }


}
