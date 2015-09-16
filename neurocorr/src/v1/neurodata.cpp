#include "neurodata.hpp"
#include "yocto/exception.hpp"


NeuroData:: ~NeuroData() throw()
{

}

static inline size_t CheckNeurons(const size_t numNeurons)
{
    if(numNeurons<=0) throw exception("NeuroData: no neurons!");
    return numNeurons;
}

static inline size_t CheckTrials(const size_t numTrials)
{
    if(numTrials<=0) throw exception("NeuroData: no trials!");
    return numTrials;
}


NeuroData:: NeuroData(const size_t numNeurons,
                      const size_t numTrials,
                      const size_t maxInput) :
neurons(CheckNeurons(numNeurons)),
trials(CheckTrials(numTrials)),
trains(neurons*trials),
raw_input(trains,1+maxInput)
{

}

RArray<double> & NeuroData:: get_raw_input(size_t neuronIndex, size_t trialIndex) throw()
{
    assert(neuronIndex<neurons);
    assert(trialIndex <trials);

    return raw_input[ neuronIndex * trials + trialIndex ];
}


#include "neuron.hpp"

void Neuron:: buildFrom( NeuroData &data, const size_t neuronIndex )
{
    assert(neuronIndex<data.neurons);

    // prepare memory
    const size_t num_trials = data.trials;
    this->free();
    this->resize_empty_to(num_trials);

    // fetch data
    for(size_t i=0;i<num_trials;++i)
    {
        this->Append( data.get_raw_input(neuronIndex,i) );
    }

    assert(trials==num_trials);
}

#include "trial.hpp"

void Trial:: buildFrom(NeuroData &data, const size_t trialIndex)
{
    assert(trialIndex<data.trials);
    
    // prepare memory
    const size_t num_neurons = data.neurons;
    this->free();
    this->resize_empty_to(num_neurons);

    //fetch data
    for(size_t j=0;j<num_neurons;++j)
    {
        this->Append( data.get_raw_input(j,trialIndex) );
    }

    assert(neurons==num_neurons);
}


