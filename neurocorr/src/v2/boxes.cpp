#include "boxes.hpp"

Boxes:: ~Boxes() throw()
{
}

Boxes:: Boxes(const Real usr_scale, const size_t nboxes) :
Object(usr_scale),
_Boxes(nboxes),
J(0),
pPHI(0),
bmgr(),
prod(0),
kMix(this,&Boxes::evalMixed)
{
}
