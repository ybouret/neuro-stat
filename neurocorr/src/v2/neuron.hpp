#ifndef NC_NEURON_INCLUDED
#define NC_NEURON_INCLUDED 1

#include "train.hpp"

class Neuron : public Object, public Trains
{
public:
    
    virtual ~Neuron() throw();
    explicit Neuron(const double        usr_scale,
                    const Matrix<Real> &neurodata,
                    const size_t        trialStart,
                    const size_t        trialCount);

private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(Neuron);
};

typedef slots_of<Neuron> NeuronsBase;

class Neurons : public Object, public NeuronsBase
{
public:
    virtual ~Neurons() throw();
    

private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(Neurons);
};

#endif

