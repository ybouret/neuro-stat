#ifndef NC_PHI_INCLUDED
#define NC_PHI_INCLUDED 1

#include "cpwfn.hpp"

typedef slots_of<CPW_Function> PhiBase;

class Phi : public PhiBase
{
public:
    virtual ~Phi() throw();

    //! allocate memory and compute
    explicit Phi(const size_t extra,
                 const Real   scale,
                 const Train &train,
                 const Unit   deltaUnit);

    //! use previous memory
    void update(const Train &train, const Unit deltaUnit);

    
private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(Phi);
};


#endif
