#ifndef NC_CPW_INCLUDED
#define NC_CPW_INCLUDED 1

#include "train.hpp"
#include "yocto/container/tuple.hpp"

YOCTO_PAIR_DECL(coord,const Unit,tau,Unit,value);
YOCTO_PAIR_END();

typedef vector<coord> _CPW;

class CPW : public _CPW
{
public:
    Unit foot;
    explicit CPW() throw();
    virtual ~CPW() throw();
    CPW(const CPW &);

    void add(const Unit tau,const Unit value);

    void save(const char *filename) const;

private:
    YOCTO_DISABLE_ASSIGN(CPW);
};


#endif
