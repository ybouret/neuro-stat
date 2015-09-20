#ifndef NC_TYPES_INCLUDED
#define NC_TYPES_INCLUDED 1

#include "yocto/object.hpp"
using namespace yocto;

typedef unit_t Unit;
typedef double Real;


//! base class for integer/real convertsion
class Object : public object
{
public:
    const Real scale;
    
    //! init with usr_scale>0
    explicit Object(const Real usr_scale) throw();
    
    //! clean up
    virtual ~Object() throw();
    
    //! copy scale
    Object(const Object &other) throw();
    
    //! conversion
    Unit toUnit(const Real t) const throw();
    Real toReal(const Unit u) const throw();
    
private:
    YOCTO_DISABLE_ASSIGN(Object);
};

#endif

