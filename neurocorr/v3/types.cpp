#include "types.hpp"
#include <cmath>

Converter:: ~Converter() throw()
{
}

Converter:: Converter(const Real usr_scale) throw() :
scale(usr_scale)
{
    assert(scale>0);
}

Unit Converter::toUnit(const Real x) const throw()
{
    return Unit( floor( double(x*scale) + 0.5 ) );
}

Real Converter:: toReal(const Unit u) const throw()
{
    return u/scale;
}