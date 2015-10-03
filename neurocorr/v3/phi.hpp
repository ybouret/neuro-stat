#ifndef NC_PHI_INCLUDED
#define NC_PHI_INCLUDED 1

#include "records.hpp"
#include "cpw.hpp"

typedef slots_of<CPW> _CPW_Functions;

class CPW_Functions : public _CPW_Functions
{
public:
    const Train &train;
    explicit CPW_Functions(const Train &attached, const size_t extra);
    virtual ~CPW_Functions() throw();

    void compute(const Unit delta);

private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(CPW_Functions);
};


//! sextuple of computations to do for mixed terms
YOCTO_SEXTUPLE_DECL(Mix,const size_t,i,const size_t,k,const size_t,l,const size_t,m,const size_t,I_i_k,const size_t,I_l_m);
YOCTO_SEXTUPLE_END();
typedef slots_of<Mix> Mixed;

typedef slots2D_of<CPW_Functions> _PHI;

class PHI : public _PHI
{
public:
    const size_t &trials;    //!< alias to _PHI.rows
    const size_t &neurones;  //!< alias to _PHI.cols
    const size_t  K;         //!< #subfunctions
    const size_t  NK;        //!< neurones*K
    const size_t  dim;       //!< 1+neurones*K, dimensions for matrices
    Mixed         mixed;     //!< the mixed terms for G dim*(dim-1)/2
    const size_t  maxCount;

    virtual ~PHI() throw();
    explicit PHI(const Records &records,const size_t extra);

    void compute(const Unit delta, Crew *para);


private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(PHI);
    mutable Unit delta_;
    Kernel       run;
    // computing using delta_
    void computeOn(Context &ctx);
    void     prepareMixed();
};

#endif

