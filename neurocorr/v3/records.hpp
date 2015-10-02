#ifndef NC_RECORDS_INCLUDED
#define NC_RECORDS_INCLUDED 1

#include "train.hpp"
#include "yocto/sequence/slots2d.hpp"

typedef auto_ptr<Train>      TrainPtr;
typedef slots2D_of<TrainPtr> _Records;

class Records : public _Records
{
public:
    explicit Records(const size_t num_trains,
                     const size_t num_neurons);

    virtual ~Records() throw();
    
private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(Records);
};


#endif

