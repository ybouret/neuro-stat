#ifndef NC_RECORDS_INCLUDED
#define NC_RECORDS_INCLUDED 1

#include "train.hpp"
#include "yocto/sequence/slots2d.hpp"

//typedef auto_ptr<Train>      TrainPtr;
typedef slots2D_of<Train> _Records;

class Records : public Converter, public _Records
{
public:
    explicit Records(const Real          usrScale,
                     const Matrix<Real> &data,
                     const size_t        numNeurones);

    virtual ~Records() throw();

    const size_t &trials;    //!< alias to _Records.rows
    const size_t &neurones;  //!< alias to _Records.cols
    const size_t  maxCount;
    const Unit    tauMin;
    const Unit    tauMax;
    static Records *CreateRandom(const size_t numTrials,
                                 const size_t numNeurones,
                                 const size_t maxSpikes,
                                 const size_t pace);
    void display() const;

private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(Records);
};


#endif

