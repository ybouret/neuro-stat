#include "neuron.hpp"


Neuron:: ~Neuron() throw() {}

Neuron:: Neuron(size_t nTrials) :
Records(nTrials),
trials(size)
{
}


Neurons:: ~Neurons() throw() {}

Neurons:: Neurons(size_t numNeurons) : NeuronsBase(numNeurons)
{

}

