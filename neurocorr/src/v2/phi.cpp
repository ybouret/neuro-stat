#include "phi.hpp"


PhiPerTrain:: ~PhiPerTrain() throw()
{

}

PhiPerTrain:: PhiPerTrain(const size_t num_extra,
                          const Train &the_train ) :
PhiPerTrainBase(1+num_extra),
train(the_train)
{
    PhiPerTrainBase &self = *this;
    const Real scale = train.scale;
    for(size_t i=0;i<=num_extra;++i)
    {
        self.append<Real>(scale);
    }

    const size_t np = train.size() * 2;
    for(size_t i=0;i<size;++i)
    {
        self[i].ensureCapacity(np);
    }
}

void PhiPerTrain:: compute(const Unit deltaUnit)
{
    PhiPerTrainBase &self = *this;
    CPW_Function &phi0 = self[0];
    phi0.buildFrom(train,deltaUnit);
    for(size_t i=1;i<size;++i)
    {
        self[i].copyAndShift(phi0,deltaUnit);
    }
}

////////////////////////////////////////////////////////////////////////////////

PhiPerNeuron:: ~PhiPerNeuron() throw()
{
}

PhiPerNeuron:: PhiPerNeuron(const size_t extra,
                            const Neuron &neuron) :
PhiPerNeuronBase(neuron.trials)
{
    for(size_t i=0;i<neuron.trials;++i)
    {
        append<size_t,const Train &>(extra,neuron[i]);
    }

}
