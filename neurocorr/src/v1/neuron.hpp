#ifndef NC_NEURON_INCLUDED
#define NC_NEURON_INCLUDED 1

#include "record.hpp"

//! Neuron : each Record is a Trial.
class Neuron : public Records
{
public:
    explicit Neuron(size_t nTrials);
    virtual ~Neuron() throw();

    const size_t &trials; //!< alias Records::size

private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(Neuron);
};


#endif

