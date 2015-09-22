#ifndef NC_PHI_INCLUDED
#define NC_PHI_INCLUDED 1

#include "cpwfn.hpp"

typedef slots_of<CPW_Function> PhiBase;

class Phi : public PhiBase
{
public:
    virtual ~Phi() throw();
    explicit Phi(const size_t extra,
                 const Train &train,
                 const Unit   deltaUnit );

private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(Phi);
};


#endif
