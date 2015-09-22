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


void CPW_Function:: buildFrom(const RArray<Unit> &train,
                              const Unit          deltaUnit,
                              const bool          cleanUp)
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

    assert(2*n==coords.size());

    if(cleanUp) removeEmptyIntervals();
}

void CPW_Function:: removeEmptyIntervals() throw()
{
    // the first and last point cannot be multiple
    size_t i = size();
    if(i>3)
    {
        --i;
        size_t im = i-1;
        while(im>1)
        {
            Coord &Cm = coords[im];
            Coord &C  = coords[i];
            if(Cm.tau>=C.tau)
            {
                std::cerr << "Should remove tau=" << Cm.tau << std::endl;
                const Coord &Cp = coords[i+1];
                memmove(&Cm, &Cp, sizeof(Coord) * (coords.size()-i) );
                coords.pop_back();
                coords.pop_back();
            }
            --im;
            --i;
        }
    }
}

void CPW_Function:: clear() throw()
{
    foot = 0;
    coords.free();
}




#include "yocto/exception.hpp"

static inline Real __check_same_scales(const CPW_Function &lhs,
                                       const CPW_Function &rhs)
{
    const Real l = lhs.scale;
    const Real r = rhs.scale;
    if( 0 != __compare(l,r) )
    {
        throw exception("CPW Functions have different scales !");
    }
    return lhs.scale;
}

void CPW_Function:: product(const CPW_Function &lhs, const CPW_Function &rhs)
{
    assert(this != &lhs );
    assert(this != &rhs );
    __check_same_scales(lhs,rhs);
    coords.free();
    const size_t nL = lhs.size();
    const size_t nR = rhs.size();
    coords.ensure(nL+nR);
    Real lvalue = lhs.foot;
    Real rvalue = rhs.foot;
    foot = lvalue*rvalue;

    // interleaved scan
    size_t iL = 1;
    size_t iR = 1;
    while( (iL<=nL) && (iR<=nR) )
    {
        const Coord & lC = lhs.coords[iL];
        const Coord & rC = rhs.coords[iR];
        const Unit    l  = lC.tau;
        const Unit    r  = rC.tau;
        if(l<r)
        {
            lvalue = lC.value;
            const Coord C(l,lvalue*rvalue);
            coords.push_back(C);
            ++iL;
        }
        else
        {
            if(r<l)
            {
                rvalue = rC.value;
                const Coord C(r,lvalue*rvalue);
                coords.push_back(C);
                ++iR;
            }
            else
            {
                lvalue = lC.value;
                rvalue = rC.value;
                const Coord C(l,lvalue*rvalue);
                coords.push_back(C);
                ++iL;
                ++iR;
            }
        }
    }
    assert( (iL>=nL) || (iR>=nR) );

    // append remaining values
    for(;iL<=nL;++iL)
    {
        Coord C( lhs.coords[iL] );
        C.value *= rvalue;
        coords.push_back(C);
    }

    for(;iR<=nR;++iR)
    {
        Coord C( rhs.coords[iR] );
        C.value *= lvalue;
        coords.push_back(C);
    }

    assert(coords.size()<=nL+nR);

}

void CPW_Function:: copyAndShift(const CPW_Function &fn,
                                 const Unit          deltaUnit)
{
    assert(this != &fn );
    __check_same_scales(*this,fn);
    coords = fn.coords;
    shiftBy(deltaUnit);
}


CPW_Function:: CPW_Function(const CPW_Function &lhs,
                            const CPW_Function &rhs ) :
Object( __check_same_scales(lhs,rhs) ),
foot(0),
coords()
{
    product(lhs,rhs);
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

