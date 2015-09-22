#include "cpwfn.hpp"
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

void CPW_Function:: product(const CPW_Function &lhs,
                            const CPW_Function &rhs)
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

