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
                       const size_t maxInput);

    virtual ~NeuroData() throw();

    const size_t          neurons;
    const size_t          trials;
    const size_t          trains;   //!< neurons*trials

    RArray<double> & get_raw_input(size_t neuronIndex, size_t trialIndex) throw();
    

private:
    CMatrix<double>       raw_input;
    YOCTO_DISABLE_COPY_AND_ASSIGN(NeuroData);
};

#endif
