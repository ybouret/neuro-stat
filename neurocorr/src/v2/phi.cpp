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
    for(size_t i=1;i<size;++i)
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
    assert(size==neuron.trials);
}

void PhiPerNeuron:: update(const Unit deltaUnit)
{
    assert(neuron.trials==size);
    PhiPerNeuronBase &self = *this;
    for(size_t i=0;i<size;++i)
    {
        PhiPerTrain &phi   = self[i];
        const Train &train = neuron[i];
        phi.update(train,deltaUnit);
        //self[i].update(neuron[i],deltaUnit);
    }
}


////////////////////////////////////////////////////////////////////////////////
PhiPerNeurons:: ~PhiPerNeurons() throw()
{

}


//namespace {

YOCTO_QUAD_DECL(BuildProxy,const size_t,extra,const Neuron *,neuron,const Unit,delta,PhiPerNeuronPtr *,handle);

void operator()(lockable &access)
{
    assert(handle);
    assert(neuron);
    handle->reset( new PhiPerNeuron(extra,*neuron,delta) );
}

YOCTO_QUAD_END();

//}
#include "yocto/exception.hpp"

PhiPerNeurons:: PhiPerNeurons(const size_t       extra,
                              const Neurons     &neurons,
                              const Unit         deltaUnit,
                              threading::engine *parallel) :
PhiPerNeuronsBase(neurons.size)
{
    PhiPerNeuronsBase &self = *this;

    // prepare memory
    for(size_t i=0;i<neurons.size;++i)
    {
        self.append<PhiPerNeuron*>(NULL);
    }
    assert(size==neurons.size);

    if(parallel)
    {
        parallel->failed = 0;
        for(size_t i=0;i<size;++i)
        {
            BuildProxy                   proxy(extra,&neurons[i],deltaUnit,&self[i]);
            const threading::engine::job J(proxy);
            parallel->enqueue(proxy);
        }
        parallel->flush();
        if(parallel->failed)
        {
            throw exception("Failed on Building Phi");
        }
    }
    else
    {
        for(size_t i=0;i<size;++i)
        {
            const Neuron &neuron = neurons[i];
            self[i].reset( new PhiPerNeuron(extra,neuron,deltaUnit) );
        }
    }


}

YOCTO_PAIR_DECL(UpdateProxy,const Unit,delta,PhiPerNeuronPtr*,handle);

void operator()(lockable &)
{
    (**handle).update(delta);
}

YOCTO_PAIR_END();

void PhiPerNeurons:: update(const Unit deltaUnit, threading::engine *parallel)
{
    if(parallel)
    {
        parallel->failed = 0;
        for(size_t i=0;i<size;++i)
        {
            UpdateProxy                   proxy(deltaUnit,& (*this)[i]);
            const threading::engine::job J(proxy);
            parallel->enqueue(proxy);
        }
        parallel->flush();
        if(parallel->failed)
        {
            throw exception("Failed on Updating Phi");
        }

    }
    else
    {
        for(size_t i=0;i<size;++i)
        {
            (*this)[i]->update(deltaUnit);
        }
    }
}


