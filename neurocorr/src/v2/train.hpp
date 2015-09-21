#ifndef NC_TRAIN_INCLUDED
#define NC_TRAIN_INCLUDED 1

#include "types.hpp"
#include "yocto/R/IR.hpp"
#include "yocto/container/slots.hpp"


class Train : public Object, public CVector<Unit>
{
public:

    //! fetch and convert data to Units
    explicit Train(const Real          scale,
                   const Matrix<Real> &neurodata,
                   const size_t        iTrain);
    

    virtual ~Train() throw();
    
private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(Train);
};

typedef slots_of<Train> TrainsBase;

class Trains : public slots_of<Train>
{
public:
    explicit Trains( const size_t num_trials );
    virtual ~Trains() throw();

    const size_t &trials;     //!< current stored trials
    const size_t &max_trials; //!< from initial num_trials

    void buildFrom(const double        scale,
                   const Matrix<Real> &neurodata,
                   const size_t        trialOffset);


private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(Trains);
};


#endif
