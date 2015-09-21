#ifndef NC_TRAIN_INCLUDED
#define NC_TRAIN_INCLUDED 1

#include "types.hpp"
#include "yocto/R/IR.hpp"

class Train : public Object, public LArray<Unit>
{
public:
    virtual ~Train() throw();
    
private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(Train);
};

#endif
