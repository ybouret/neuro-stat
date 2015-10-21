#ifndef NC_TYPES_INCLUDED
#define NC_TYPES_INCLUDED

#include "yocto/threading/crew.hpp"
#include "yocto/math/matrices.hpp"

using namespace yocto;


typedef unit_t Unit;
typedef double Real;

inline Unit UnitAbs(const Unit x)  throw()
{
    return (x < 0) ? -x : x;
}

//! convertion Real<->Unit
class Converter
{
public:
    explicit Converter(const Real usr_scale) throw();
    virtual ~Converter() throw();

    const Real scale;

    //! convention is 'to nearest'
    Unit toUnit(const Real x) const throw();

    //! u/scale
    Real toReal(const Unit u) const throw();

private:
    YOCTO_DISABLE_ASSIGN(Converter);
};

typedef array<Unit>             UArray;
typedef vector<Unit>            UVector;


#endif

