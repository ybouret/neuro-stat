#include "cpwfn.hpp"


CPW_Function:: ~CPW_Function() throw()
{
}

CPW_Function:: CPW_Function(const Real usr_scale) throw() :
Object(usr_scale),
foot(0),
coords()
{
}


void CPW_Function:: shiftBy(const Unit deltaUnit) throw()
{
    for(size_t i=coords.size();i>0;--i)
    {
        coords[i].tau += deltaUnit;
    }
}

CPW_Function:: CPW_Function(const CPW_Function &fn, const Unit deltaUnit) :
Object(fn),
foot(fn.foot),
coords(fn.coords)
{
    assert(this->is_same_that(fn));
    shiftBy(deltaUnit);
}



void CPW_Function:: insert(const Unit tau, const Unit value)
{
    const Coord C(tau,value);
    coords.push_back(C);

    // move into place
    for(size_t i=coords.size(),im=i-1;im>0;--i,--im)
    {
        Coord &Cm = coords[im];
        Coord &C  = coords[i];
        if(C.tau<Cm.tau)
        {
            bswap(C,Cm);
        }
        else
        {
            break;
        }
    }
}

size_t CPW_Function:: size() const throw() { return coords.size(); }

const Coord & CPW_Function:: operator[](const size_t indx) const throw()
{
    assert(indx>0);
    assert(indx<=size());
    return coords[indx];
}

const Unit & CPW_Function:: value(const size_t indx ) const throw()
{
    assert(indx>0);
    assert(indx<=size());
    return coords[indx].value;
}

Unit & CPW_Function:: value(const size_t indx )  throw()
{
    assert(indx>0);
    assert(indx<=size());
    return coords[indx].value;
}


const Coord & CPW_Function:: front() const throw()
{
    assert(size()>0);
    return coords.front();
}

const Coord & CPW_Function:: back() const throw()
{
    assert(size()>0);
    return coords.back();
}



void CPW_Function:: clear() throw()
{
    foot = 0;
    coords.free();
}


void CPW_Function:: ensureCapacity(size_t n)
{
    coords.ensure(n);
}

bool CPW_Function:: is_same_that(const CPW_Function &F) const throw()
{
    if(foot!=F.foot)        return false;
    const size_t np = size();
    if(F.size() != np ) return false;
    for(size_t i=np;i>0;--i)
    {
        const Coord &A = coords[i];
        const Coord &B = F.coords[i];
        if(A.tau!=B.tau)     return false;
        if(A.value!=B.value) return false;
    }

    //everything is equal
    return true;
}

#include "yocto/ios/ocstream.hpp"


void CPW_Function:: saveTo(const char *filename) const
{
    ios::wcstream       fp(filename);
    const CPW_Function &fn = *this;
    if(fn.size()>0)
    {
        fp("%ld %ld\n", long(fn[1].tau-1),long(fn.foot));
        fp("%ld %ld\n", long(fn[1].tau), long(fn.foot));
        for(size_t i=1;i<fn.size();++i)
        {
            fp("%ld %ld\n", long(fn[i].tau),   long(fn[i].value));
            fp("%ld %ld\n", long(fn[i+1].tau), long(fn[i].value));
        }
        fp("%ld %ld\n", long(fn.back().tau),   long(fn.back().value));
        fp("%ld %ld\n", long(fn.back().tau+1), long(fn.back().value));
        
    }
}

