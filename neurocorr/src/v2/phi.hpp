#ifndef NC_PHI_INCLUDED
#define NC_PHI_INCLUDED 1

#include "cpwfn.hpp"
#include "yocto/ptr/auto.hpp"
#include "yocto/threading/engine.hpp"
#include "yocto/threading/crew.hpp"
#include "neuron.hpp"

typedef slots_of<CPW_Function> PhiPerTrainBase;

//! Phi for ONE ATTACHED TRAIN, minimal granularity
class PhiPerTrain : public PhiPerTrainBase
{
public:
    //! allocate memory, 2*train.size
    explicit PhiPerTrain(const size_t extra,
                         const Train &the_train);
    virtual ~PhiPerTrain() throw();

    const Train &train;
    void compute(const Unit deltaUnit);

private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(PhiPerTrain);
};



typedef slots_of<PhiPerTrain> PhiPerNeuronBase;

//! Phi's for ONE NEURON
class PhiPerNeuron : public PhiPerNeuronBase
{
public:
    explicit PhiPerNeuron(const size_t extra,
                          const Neuron &neuron);
    virtual ~PhiPerNeuron() throw();

private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(PhiPerNeuron);
};

typedef slots_of<PhiPerNeuron> PhiPerNeuronsBase;

//! Phi's for SOME NEURONS
class PhiPerNeurons : public PhiPerNeuronsBase
{
public:
    explicit PhiPerNeurons(size_t extra, const Neurons &neurons);
    virtual ~PhiPerNeurons() throw();

    size_t trains() const throw();

private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(PhiPerNeurons);
    vector<PhiPerTrain*> phi;
};



#endif
