#include "neuron.hpp"
#include "yocto/exception.hpp"

Neuron:: Neuron(const double        usr_scale,
                const Matrix<Real> &neurodata,
                const size_t        trialStart,
                const size_t        trialCount) :
Object(usr_scale),
Trains(trialCount)
{
    buildFrom(scale,neurodata,trialStart);
}


Neuron:: ~Neuron() throw()
{

}

Neurons:: ~Neurons() throw()
{
}

Neurons:: Neurons(const Real          usr_scale,
                  const Matrix<Real> &neurodata,
                  const size_t        num_neurons) :
Object(usr_scale),
NeuronsBase(num_neurons)
{
    const size_t num_trains = neurodata.rows;
    if(0 != (num_trains%num_neurons) )
        throw exception("invalid neurodata #trains");

    const size_t trialCount = num_trains/num_neurons;
    if(trialCount<=0)
        throw exception("no valid trials for neurodata");

    for(size_t i=0;i<num_neurons;++i)
    {
        const size_t trialStart = i*trialCount;
        append<Real,const Matrix<Real>&, size_t, size_t>(scale,neurodata,trialStart,trialCount);
    }
}
