#ifndef NC_NEURODATA_INCLUDED
#define NC_NEURODATA_INCLUDED 1

#include "yocto/R/IR.hpp"
#include "neuron.hpp"
#include "trial.hpp"


//! Neuronal Data: input matrix
/**
 Once the matrix is loaded, call setup to gather Neurons and Trials.
 */
class NeuroData
{
public:
    //! prepare the matrix: (numNeurons*numTrials) x (1+maxInput)
    explicit NeuroData(const size_t numNeurons,
                       const size_t numTrials,
                       const size_t maxInputs);

    //! default destructor
    virtual ~NeuroData() throw();

    const size_t          neurons;               //!< #neurons in this data
    const size_t          trials;                //!< #trials  in this data
    const size_t          trains;                //!< neurons*trials
    const size_t          max_spikes_per_train;  //!< max spikes per train

    const Neurons         neuron;                //!< database of neurons, computed by setup()
    const Trials          trial;                 //!< database of trials,  computed by setup()

    void setup(); //!< build neurons and trials

    //! low level access to raw data
    RArray<double> & get_raw_input(size_t neuronIndex, size_t trialIndex) throw();

    //! low level access to raw data
    RArray<double> & get_raw_input(size_t trainIndex) throw();
    
private:
    CMatrix<double>       raw_input;
    YOCTO_DISABLE_COPY_AND_ASSIGN(NeuroData);
};

#endif
