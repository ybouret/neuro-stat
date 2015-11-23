#include "cpw.hpp"
#include "yocto/sort/quick.hpp"

CPW:: ~CPW() throw()
{
}

CPW:: CPW(size_t max_points) :
_CPW(max_points),
foot(0)
{

}

void CPW:: insert(const Unit tau, const Unit value) throw()
{
    assert(size<capacity);
    const coord C(tau,value);
    push_back(C);
}

void CPW:: shiftFrom(const CPW &other, const Unit delta)
{
    free();
    const size_t np = other.size;
    if(capacity<other.size)
    {
        //std::cerr << "MUST RESIZE!!" << std::endl;
        resize_empty_to(np);
    }

    for(size_t i=0;i<np;++i)
    {
        const coord &C = other[i];
        insert(C.tau+delta,C.value);
    }

    assert(size==other.size);
}

#include "yocto/sequence/lw-array.hpp"

void CPW:: sort() throw()
{
    const size_t np = size;
    if(np>1)
    {
        _CPW  &self = *this;
        lw_array<coord> arr( &self[0], np );
        quicksort(arr,coord::compare);
    }
}

#include "yocto/ios/ocstream.hpp"

void CPW:: save(const char *filename) const
{
    ios::wcstream fp(filename);
    const size_t   np   = size;
    const _CPW    &self = *this;

    if(np>0)
    {
        fp("%ld %ld\n", long(self[0].tau)-1, long(foot));
        fp("%ld %ld\n", long(self[0].tau), long(foot));
        for(size_t i=0;i<np-1;++i)
        {
            fp("%ld %ld\n", long(self[i].tau), long(self[i].value));
            fp("%ld %ld\n", long(self[i+1].tau), long(self[i].value));
        }
        fp("%ld %ld\n", long(self[np-1].tau),long(self[np-1].value));
        fp("%ld %ld\n", long(self[np-1].tau)+1,long(self[np-1].value));
    }
    else
    {
        fp("-1 %ld\n1 %ld\n", long(foot), long(foot));
    }
}