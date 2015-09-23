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

////////////////////////////////////////////////////////////////////////////////
PhiPerNeurons:: ~PhiPerNeurons() throw() {}



PhiPerNeurons:: PhiPerNeurons(size_t extra, const Neurons &neurons) :
PhiPerNeuronsBase(neurons.size),
delta(0),
phi(),
run(this,&PhiPerNeurons::computePara)
{
    size_t num_phi_per_train = 0;
    for(size_t i=0;i<neurons.size;++i)
    {
        const Neuron &nn = neurons[i];
        num_phi_per_train += nn.trials;
        append<size_t,const Neuron &>(extra,nn);
    }
    assert(size==neurons.size);

    phi.make(num_phi_per_train,NULL);
    size_t j=0;
    for(size_t i=0;i<size;++i)
    {
        PhiPerNeuron &sub = (*this)[i];
        for(size_t k=0;k<sub.size;++k)
        {
            PhiPerTrain &pp = sub[k];
            phi[++j] = &pp;
        }
    }
    assert(num_phi_per_train==j);
}

size_t PhiPerNeurons:: trains() const throw()
{
    return phi.size();
}

void PhiPerNeurons:: compute(const Unit deltaUnit, threading::crew *para)
{
    if(para)
    {
        delta = deltaUnit;
        (*para)(run);
    }
    else
    {
        for(size_t i=phi.size();i>0;--i)
        {
            phi[i]->compute(deltaUnit);
        }
    }
}

#include "yocto/code/unroll.hpp"
#define NC_UPDATE_PHI(I) \
phi[I]->compute(delta)

void PhiPerNeurons:: computePara(threading::crew::context &ctx)
{
    size_t offset = 1;
    size_t length = phi.size();
    ctx.split(offset, length);
    YOCTO_LOOP_FUNC(length,NC_UPDATE_PHI,offset);
}
