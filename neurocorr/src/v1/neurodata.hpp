#ifndef NC_NEURODATA_INCLUDED
#define NC_NEURODATA_INCLUDED 1

#include "yocto/R/IR.hpp"
using namespace yocto;

//! Neuronal Data: big input matrix + info
class NeuroData
{
public:
    //! prepare the matrix: (numNeurons*numTrials) x (1+maxInput)
    explicit NeuroData(const size_t numNeurons,
                       const size_t numTrials,
                       const size_t maxInputs);

    //! default destructor
    virtual ~NeuroData() throw();

    const size_t          neurons;
    const size_t          trials;
    const size_t          trains;                //!< neurons*trials
    const size_t          max_spikes_per_train;  //!< max spikes per train

    RArray<double> & get_raw_input(size_t neuronIndex, size_t trialIndex) throw();
    RArray<double> & get_raw_input(size_t trainIndex) throw();

private:
    CMatrix<double>       raw_input;
    YOCTO_DISABLE_COPY_AND_ASSIGN(NeuroData);
};

#endif
