#include "train.hpp"
#include "yocto/exception.hpp"

Train:: ~Train() throw()
{
}

static inline
size_t __get_length_of(const Matrix<Real> &neurodata,
                       const size_t        iTrain)
{
    assert(neurodata.rows>0);
    assert(neurodata.cols>0);

    if(iTrain>=neurodata.rows)
    {
        throw exception("invalid train index: %u >= %u", unsigned(iTrain), unsigned(neurodata.rows) );
    }


    const size_t num_spikes = size_t(neurodata(iTrain,0));
    if(num_spikes>neurodata.rows-1)
    {
        throw exception("corrupted num spikes in train #%u", unsigned(iTrain));
    }

    return num_spikes;
}



Train:: Train(const Real          scale,
              const Matrix<Real> &neurodata,
              const size_t        iTrain) :
Object(scale),
CVector<Unit>( __get_length_of(neurodata,iTrain) )
{
    const size_t n = size();
    for(size_t i=0;i<n;++i)
    {
        (*this)[i] = toUnit(neurodata(iTrain,i+1));
    }
}
