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


size_t CPW_Function:: find_index_for( const Unit tau ) const throw()
{
    assert(size()>1);
    const size_t n   = coords.size();
    size_t       jlo = 1;
    size_t       jhi = n;
    assert(tau  > coords[1].tau);
    assert(tau <= coords[n].tau);
    while(jhi-jlo>1)
    {
        const Unit jmid = (jlo+jhi)>>1;
        if(tau<=coords[jmid].tau)
        {
            jhi = jmid;
        }
        else
        {
            jlo = jmid;
        }
    }
    return jlo;
}


#include "yocto/ios/ocstream.hpp"


void CPW_Function:: saveTo(const char *filename) const
{
    ios::wcstream       fp(filename);
    const CPW_Function &fn = *this;
    if(fn.size()>0)
    {
        fp("%ld %ld\n", fn[1].tau-1,fn.foot);
        fp("%ld %ld\n", fn[1].tau, fn.foot);
        for(size_t i=1;i<fn.size();++i)
        {
            fp("%ld %ld\n", fn[i].tau,   fn[i].value);
            fp("%ld %ld\n", fn[i+1].tau, fn[i].value);
        }
        fp("%ld %ld\n", fn.back().tau,   fn.back().value);
        fp("%ld %ld\n", fn.back().tau+1, fn.back().value);
        
    }
}

