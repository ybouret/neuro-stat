#ifndef NC_TRAIN_INCLUDED
#define NC_TRAIN_INCLUDED 1

#include "types.hpp"
#include "yocto/sequence/vector.hpp"

//! base class for spikes train
typedef UVector _Train;

//! a vector of 1..size() units
class Train : public _Train
{
public:
    //! empty train
    explicit Train() throw();

    //! capacity only !
    explicit Train(size_t n);

    //! release memory
    virtual ~Train() throw();

    //! copy if needed
    Train(const Train &other);
    

private:
    YOCTO_DISABLE_ASSIGN(Train);
};




#endif
