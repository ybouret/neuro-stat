#include "neuron.hpp"

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
