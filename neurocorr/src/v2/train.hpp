#ifndef NC_TRAIN_INCLUDED
#define NC_TRAIN_INCLUDED 1

#include "types.hpp"
#include "yocto/R/IR.hpp"
#include "yocto/sequence/slots.hpp"

//! data in Units
class Train : public Object, public CVector<Unit>
{
public:

    //! fetch and convert data to Units
    /**
     Expecting a neuronal data format
     */
    explicit Train(const Real          scale,
                   const Matrix<Real> &neurodata,
                   const size_t        iTrain);

    //! manually create 0 filled train
    explicit Train(const Real scale, const size_t nSpike);
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

    const size_t &trials;     //!< current stored trials, alias to size
    const size_t &max_trials; //!< from initial num_trials, alias to capacity

    void buildFrom(const double        scale,
                   const Matrix<Real> &neurodata,
                   const size_t        trialOffset);


private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(Trains);
};


#endif
