#ifndef NC_RECORDS_INCLUDED
#define NC_RECORDS_INCLUDED 1

#include "train.hpp"
#include "yocto/container/slots2d.hpp"
#include "yocto/ptr/auto.hpp"

typedef auto_ptr<Train>      TrainPtr;
typedef slots2D_of<TrainPtr> RecordsBase;

class Records : public RecordsBase
{
public:
    const size_t &trials;   //!< alias to rows
    const size_t &neurones; //!< alias to cols
    explicit Records(const Real          scale,
                     const Matrix<Real> &neurodata,
                     const size_t        num_neurons);
    virtual ~Records() throw();

    void displayNeurones() const;
    
private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(Records);
};

#endif
