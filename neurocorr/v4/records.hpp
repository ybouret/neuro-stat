#ifndef NC_RECORDS_INCLUDED
#define NC_RECORDS_INCLUDED 1

#include "train.hpp"
#include "yocto/container/matrix.hpp"

typedef matrix<Train> _Records;

class Records : public Converter, public _Records
{
public:
    //! convert neuro data matrix
    explicit Records(const Real             scal,
                     const matrix_of<Real> &data,
                     const size_t           num_neurones);

    //! release all memory
    virtual ~Records() throw();

    const size_t &trials;
    const size_t &neurones;
    const size_t  maxCount;
    const Unit    minTau;
    const Unit    maxTau;

    static Records *GenerateRandom(const size_t num_neurones,
                                   const size_t num_trials,
                                   const size_t max_spikes,
                                   const size_t pace);

    void display() const;
    
private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(Records);
};

#endif
