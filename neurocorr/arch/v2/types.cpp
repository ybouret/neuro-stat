#include "types.hpp"
#include "yocto/math/types.hpp"

Object:: ~Object() throw()
{
}

Object:: Object( const Object &other ) throw() :
scale(other.scale)
{
    assert(scale>0);
}

Object:: Object(const Real usr_scale) throw() :
scale(usr_scale)
{
    assert(scale>0);
}

Unit Object:: toUnit(const Real t) const throw()
{
    static const Real half(0.5);
    return Unit( math::Floor(t*scale+half) );
}

Real Object:: toReal(const Unit u) const throw()
{
    return Real(u)/scale;
}

int Object:: CompareUnit(const Unit &lhs, const Unit &rhs) throw()
{
    return lhs-rhs;
}
