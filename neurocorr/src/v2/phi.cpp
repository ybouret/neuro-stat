#include "phi.hpp"

PhiPerTrain:: ~PhiPerTrain() throw()
{
}

PhiPerTrain:: PhiPerTrain(const size_t extra,
                          const Real   scale,
                          const Train &train,
                          const Unit   deltaUnit) :
PhiPerTrainBase(1+extra)
{
    PhiPerTrainBase &self = *this;

    //create 'mother phi'
    self.append<Real>(scale);
    CPW_Function &phi0 = self[0];
    phi0.buildFrom(train, deltaUnit, true);

    // create shifted functions
    for(size_t i=1;i<=extra;++i)
    {
        const Unit delta = i*deltaUnit;
        self.append<const CPW_Function &,Unit>(phi0,delta);
    }

}


void PhiPerTrain:: update(const Train &train, const Unit deltaUnit)
{
    assert(size>0);
    PhiPerTrainBase &self = *this;
    CPW_Function    &phi0 = self[0];
    phi0.buildFrom(train, deltaUnit, true);
    for(size_t i=1;i<=size;++i)
    {
        const Unit    delta = i*deltaUnit;
        CPW_Function &phi   = self[i];
        phi.copyAndShift(phi0,delta);
    }
}

////////////////////////////////////////////////////////////////////////////////
PhiPerNeuron:: ~PhiPerNeuron() throw()
{
}

PhiPerNeuron:: PhiPerNeuron(const size_t extra,
                            const Neuron &nn,
                            const Unit   deltaUnit ) :
PhiPerNeuronBase(nn.trials),
neuron(nn)
{
    for(size_t i=0;i<neuron.trials;++i)
    {
        append<size_t, Real, const Train &, Unit>(extra,
                                                  neuron.scale,
                                                  neuron[i],
                                                  deltaUnit);
    }
}

void PhiPerNeuron:: update(const Unit deltaUnit)
{
    assert(neuron.trials==size);
    PhiPerNeuronBase &self = *this;
    for(size_t i=0;i<size;++i)
    {
        self[i].update(neuron[i],deltaUnit);
    }
}


////////////////////////////////////////////////////////////////////////////////
PhiPerNeurons:: ~PhiPerNeurons() throw()
{

}

PhiPerNeurons:: PhiPerNeurons(const size_t extra, const Neurons &neurons, const Unit deltaUnit) :
PhiPerNeuronsBase(neurons.size)
{
    PhiPerNeuronsBase &self = *this;
    for(size_t i=0;i<neurons.size;++i)
    {
        const Neuron &neuron = neurons[i];
        self.append<size_t,const Neuron &,Unit>(extra, neuron, deltaUnit);
    }

}

