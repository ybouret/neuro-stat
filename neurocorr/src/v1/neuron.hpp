#ifndef NC_NEURON_INCLUDED
#define NC_NEURON_INCLUDED 1

#include "record.hpp"

//! forward declaration
class NeuroData;

//! Neuron : each Record is a Trial.
class Neuron : public Records
{
public:
    explicit Neuron(size_t nTrials);
    virtual ~Neuron() throw();

    const size_t &trials; //!< alias Records::size

    void buildFrom( NeuroData &data, const size_t neuronIndex );
    
private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(Neuron);
};


#endif

