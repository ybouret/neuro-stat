#ifndef NC_PHI_INCLUDED
#define NC_PHI_INCLUDED 1

#include "cpwfn.hpp"
#include "yocto/ptr/auto.hpp"
#include "yocto/threading/engine.hpp"

typedef slots_of<CPW_Function> PhiPerTrainBase;

//! a set of Constant PieceWise functions for ONE TRAIN of spikes
class PhiPerTrain : public PhiPerTrainBase
{
public:
    virtual ~PhiPerTrain() throw();

    //! allocate memory and compute 1+extra function
    explicit PhiPerTrain(const size_t extra,
                         const Real   scale,
                         const Train &train,
                         const Unit   deltaUnit);

    //! use previous memory
    void update(const Train &train, const Unit deltaUnit);


private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(PhiPerTrain);
};

#include "neuron.hpp"
typedef slots_of<PhiPerTrain> PhiPerNeuronBase;


//! a set of Phi functions for ONE Neuron
class PhiPerNeuron : public PhiPerNeuronBase
{
public:
    const Neuron &neuron;
    explicit PhiPerNeuron(const size_t extra,const Neuron &nn,const Unit deltaUnit);
    virtual ~PhiPerNeuron() throw();

    void update(const Unit deltaUnit);

private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(PhiPerNeuron);
};


//! a set of Phi functions for SOME NEURONS
typedef auto_ptr<PhiPerNeuron>     PhiPerNeuronPtr;
typedef slots_of<PhiPerNeuronPtr>  PhiPerNeuronsBase;

class PhiPerNeurons : public PhiPerNeuronsBase
{
public:
    virtual ~PhiPerNeurons() throw();
    explicit PhiPerNeurons(const size_t       extra,
                           const Neurons     &neurons,
                           const Unit         deltaUnit,
                           threading::engine *parallel);
    void update(const Unit deltaUnit,
                threading::engine *parallel);

private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(PhiPerNeurons);
};


#endif
