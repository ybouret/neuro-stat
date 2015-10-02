#include "train.hpp"

Train:: ~Train() throw()
{
}

Train:: Train(const size_t np) : _Train(np)
{
}

#include "yocto/ios/ocstream.hpp"

void Train:: save(const char *filename) const
{
    ios::wcstream fp(filename);
    for(size_t i=0;i<size();++i)
    {
        fp("%ld 0\n", long((*this)[i]) );
    }

}
