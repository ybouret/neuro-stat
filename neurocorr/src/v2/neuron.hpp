#ifndef NC_NEURON_INCLUDED
#define NC_NEURON_INCLUDED 1

#include "train.hpp"

class Neuron : public Trains
{
public:
    virtual ~Neuron() throw();
    
private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(Neuron);
};

#endif

