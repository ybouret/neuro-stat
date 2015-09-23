#ifndef NC_PHI_INCLUDED
#define NC_PHI_INCLUDED 1

#include "cpwfn.hpp"

typedef slots_of<CPW_Function> PhiBase;

//! a set of Constant PieceWise functions for one train of spikes
class Phi : public PhiBase
{
public:
    virtual ~Phi() throw();

    //! allocate memory and compute 1+extra function
    explicit Phi(const size_t extra,
                 const Real   scale,
                 const Train &train,
                 const Unit   deltaUnit);

    //! use previous memory
    void update(const Train &train, const Unit deltaUnit);

    
private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(Phi);
};

#include "neuron.hpp"
typedef slots_of<Phi> PhiSetBase;


//! a set of Phi functions for one neuron
class PhiSet : public PhiSetBase
{
public:
    const Neuron &neuron;
    explicit PhiSet(const size_t extra,const Neuron &nn,const Unit deltaUnit);
    virtual ~PhiSet() throw();

    void update(const Unit deltaUnit);

private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(PhiSet);
};


#endif
