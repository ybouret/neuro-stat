#ifndef NC_TRAIN_INCLUDED
#define NC_TRAIN_INCLUDED 1

#include "types.hpp"


typedef CVector<Unit> _Train;

class Train : public object, public _Train
{
public:

    explicit Train(const size_t np);
    virtual ~Train() throw();

    void save(const char *filename) const;
    
private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(Train);
};

#endif

