#ifndef NC_TRAIN_INCLUDED
#define NC_TRAIN_INCLUDED 1

#include "types.hpp"
#include "yocto/R/IR.hpp"

class Train : public Object, public CVector<Unit>
{
public:

    //! fetch and convert data to Units
    explicit Train(const Real          scale,
                   const Matrix<Real> &neurodata,
                   const size_t        iTrain);
    

    virtual ~Train() throw();
    
private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(Train);
};

#endif
