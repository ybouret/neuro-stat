#ifndef NC_TYPES_INCLUDED
#define NC_TYPES_INCLUDED

#include "yocto/R/IR.hpp"
#include "yocto/threading/crew.hpp"

using namespace yocto;


typedef unit_t Unit;
typedef double Real;

class Converter
{
public:
    explicit Converter(const Real usr_scale) throw();
    virtual ~Converter() throw();

    const Real scale;

    Unit toUnit(const Real x) const throw();
    Real toReal(const Unit u) const throw();

private:
    YOCTO_DISABLE_ASSIGN(Converter);
};

typedef threading::crew Crew;


#endif

