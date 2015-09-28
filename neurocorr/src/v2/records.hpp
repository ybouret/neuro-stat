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
    const size_t  maxCount; //!< max count of spikes in a trial
    
    explicit Records(const Real          scale,
                     const Matrix<Real> &neurodata,
                     const size_t        num_neurones);
    virtual ~Records() throw();

    void displayNeurones() const;

    //! random records with unit scaling
    static Records *CreateRandom(const size_t num_neurones,
                                 const size_t num_trials,
                                 const size_t max_spikes);


private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(Records);
};

#endif
