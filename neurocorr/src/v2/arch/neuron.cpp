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

void Neuron:: displayInfo() const
{
    std::cerr << "\t|#trials=" << trials << std::endl;
    std::cerr << "\t|_";
    for(size_t j=0;j<trials;++j)
    {
        const Train &tr = (*this)[j];
        std::cerr << "/" << tr.size();
    }
    std::cerr << "." << std::endl;
}


Neurons:: ~Neurons() throw()
{
}

Neurons:: Neurons(const Real          usr_scale,
                  const Matrix<Real> &neurodata,
                  const size_t        num_neurons) :
Object(usr_scale),
NeuronsBase(num_neurons),
trials(0)
{
    const size_t num_trains = neurodata.rows;
    if(0 != (num_trains%num_neurons) )
        throw exception("invalid neurodata #trains");

    const size_t trialCount = num_trains/num_neurons;
    if(trialCount<=0)
        throw exception("no valid trials for neurodata");

    (size_t&)trials = trialCount;
    for(size_t i=0;i<num_neurons;++i)
    {
        const size_t trialStart = i*trialCount;
        append<Real,const Matrix<Real>&, size_t, size_t>(scale,neurodata,trialStart,trialCount);
    }
}
