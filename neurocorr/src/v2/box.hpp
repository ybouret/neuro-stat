#ifndef NC_BOX_INCLUDED
#define NC_BOX_INCLUDED 1

#include "types.hpp"


class Box
{
public:
    const size_t trial;
    const Unit   tauStart;
    const Unit   tauFinal;
    const int    kind;

    explicit Box(const size_t indx,
                 const Unit   ini,
                 const Unit   end) throw();
    virtual ~Box() throw();
    Box(const Box &) throw();
    
private:
    YOCTO_DISABLE_ASSIGN(Box);
};

#endif
