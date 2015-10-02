#include "cpw.hpp"
#include "yocto/sort/quick.hpp"

CPW:: ~CPW() throw()
{
}

#include "yocto/sequence/lw-array.hpp"

void CPW:: sort() throw()
{
    const size_t np = size();
    if(np>1)
    {
        CVector<coord> &self = *this;
        lw_array<coord> arr( &self[0], np );
        quicksort(arr,coord::compare);
    }
}

#include "yocto/ios/ocstream.hpp"

void CPW:: save(const char *filename) const
{
    ios::wcstream fp(filename);
    const size_t          np = size();
    const CVector<coord> &self = *this;

    if(np>0)
    {
        fp("%ld %ld\n", long(self[0].tau), long(foot));
        for(size_t i=0;i<np-1;++i)
        {
            fp("%ld %ld\n", long(self[0].tau), long(self[0].value));
        }
    }
    else
    {
        fp("-1 %ld\n1 %ld\n", long(foot), long(foot));
    }
}