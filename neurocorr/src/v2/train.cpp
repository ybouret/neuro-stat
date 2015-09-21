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
    if(num_spikes>neurodata.cols-1)
    {
        throw exception("corrupted  #spikes=%u in train #%u", unsigned(num_spikes), unsigned(iTrain));
    }

    return num_spikes;
}



Train:: Train(const Real          scale,
              const Matrix<Real> &neurodata,
              const size_t        iTrain) :
Object(scale),
CVector<Unit>( __get_length_of(neurodata,iTrain) )
{
    //std::cerr << "Creating Train #" << iTrain << " with " << size() << " spikes" << std::endl;
    const size_t n = size();
    for(size_t i=0;i<n;++i)
    {
        (*this)[i] = toUnit(neurodata(iTrain,i+1));
    }
}

Trains:: Trains(const size_t num_trials) :
TrainsBase(num_trials),
trials(size),
max_trials(capacity)
{
    assert(capacity==max_trials);
}


Trains:: ~Trains() throw()
{
}


void Trains:: buildFrom(const double        scale,
                        const Matrix<Real> &neurodata,
                        const size_t        trialOffset)
{
    assert(0==trials);
    assert(trialOffset+max_trials<=neurodata.rows);
    for(size_t i=0;i<max_trials;++i)
    {
        const size_t iTrain = i + trialOffset;
        append<Real,const Matrix<Real>&,size_t>(scale,neurodata,iTrain);
    }
    
}

