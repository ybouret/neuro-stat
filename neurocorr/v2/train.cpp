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


Train:: Train(const Real scale, const size_t nSpike) :
Object(scale),
CVector<Unit>(nSpike)//,Tau(size(),as_capacity)
{
}



Train:: Train(const Real          scale,
              const Matrix<Real> &neurodata,
              const size_t        iTrain) :
Object(scale),
CVector<Unit>( __get_length_of(neurodata,iTrain) )//,Tau(size(),as_capacity)
{
    const size_t   n = size();
    CVector<Unit> &a = *this;
    for(size_t i=0;i<n;++i)
    {
        const Real tmp = neurodata(iTrain,i+1);
        a[i] = toUnit(tmp);
    }

    // ensure ordered and strictly increasing
    for(size_t i=0,ip=i+1;ip<n;++i,++ip)
    {
        if(a[ip]<=a[i])
        {
            throw exception("train %u: data are not ordered or scale is too small!!!", unsigned(iTrain));
        }
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

