#include "cpwfn.hpp"


CPW_Function:: ~CPW_Function() throw()
{
}

CPW_Function:: CPW_Function(const Real usr_scale, const size_t n) :
Object(usr_scale),
foot(0),
coords(n,as_capacity)
{
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
            break;
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


void CPW_Function:: buildFrom( const RArray<Unit> &train, const Unit deltaUnit )
{
    assert(deltaUnit>0);
    const size_t n = train.size();
    coords.free();
    coords.ensure(2*n);

    CVector<Unit> shift(n);
    for(size_t i=0;i<n;++i)
    {
        shift[i] = train[i] + deltaUnit;
    }

    size_t iShift = 0;
    size_t iTrain = 0;
    Unit   curr   = 0;
    foot          = 0;
    while(iTrain<n)
    {
        const Unit tt = train[iTrain];
        const Unit ts = shift[iShift];
        if(tt<=ts)
        {
            ++curr;
            const Coord C(tt,curr);
            coords.push_back(C);
            ++iTrain;
        }
        else
        {
            --curr;
            const Coord C(ts,curr);
            coords.push_back(C);
            ++iShift;
        }
    }

    while(iShift<n)
    {
        --curr;
        const Coord C(shift[iShift++],curr);
        coords.push_back(C);
    }
}
