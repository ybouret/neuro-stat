#include "types.hpp"
#include <cmath>

Converter:: ~Converter() throw()
{
}

Converter:: Converter(const Real usr_scale) :
scale(usr_scale)
{
    if(scale<1) throw exception("Converter scale is meaningless");
}

Unit Converter::toUnit(const Real x) const throw()
{
    return Unit( floor( double(x*scale) + 0.5 ) );
}

Real Converter:: toReal(const Unit u) const throw()
{
    return u/scale;
}

