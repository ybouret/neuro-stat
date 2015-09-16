#ifndef NC_TRIAL_INCLUDED
#define NC_TRIAL_INCLUDED 1

#include "record.hpp"

//! forward declaration
class NeuroData;

//! a trial with its Records=data for each neuron
class Trial : public Records
{
public:
    explicit Trial(const size_t nNeurons);
    virtual ~Trial() throw();

    const size_t &neurons; //!< alias to Records::size

    void buildFrom(NeuroData &data, const size_t trialIndex);

private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(Trial);
};

typedef dynamic_slots<Trial> TrialsBase;

class Trials : public TrialsBase
{
public:
    explicit Trials(const size_t nTrials);
    virtual ~Trials() throw();

    void buildFrom(NeuroData &data);
    
private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(Trials);
};

#endif

