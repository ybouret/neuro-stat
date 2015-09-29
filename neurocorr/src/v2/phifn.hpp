#ifndef NC_PHIFN_INCLUDED
#define NC_PHIFN_INCLUDED 1


#include "records.hpp"
#include "cpwfn.hpp"
#include "yocto/threading/crew.hpp"

typedef threading::crew Crew;
typedef Crew::kernel    Kernel;
typedef Crew::context   Context;


typedef slots_of<CPW_Function> CPW_Functions;

//! a set of CPW_Functions attached to ONE train
class PHI_Functions : public CPW_Functions
{
public:
    const Train &train; //!< attached train

    //! prepare functions and memory
    explicit PHI_Functions(const size_t extra,
                           const Train &tr);

    virtual ~PHI_Functions() throw();

    //! compute all functions
    void compute(const Unit deltaUnit);

private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(PHI_Functions);
};

typedef auto_ptr<PHI_Functions>        PHI_FunctionsPtr;
typedef slots2D_of<PHI_FunctionsPtr>   PHI_Base;

//! sextuple of computations to do
YOCTO_SEXTUPLE_DECL(Mix,const size_t,i,const size_t,k,const size_t,l,const size_t,m,const size_t,I_i_k,const size_t,I_l_m);
YOCTO_SEXTUPLE_END();

typedef slots_of<Mix> Mixed;

class PHI : public PHI_Base
{
public:
    const size_t &trials;
    const size_t &neurones;
    const size_t  K;     //!< 1+extra
    const size_t  NK;    //!< neurones*K
    const size_t  dim;   //!< 1+neurons*K, dimension for G/B/...
    Mixed         mixed; //!< (neurons*K)*(neurons*K+1)/2, all possible mixed terms

    explicit PHI(const size_t extra, const Records &records);
    virtual ~PHI() throw();

    //! compute the set of Phi functions for a given delta
    /**
     this can be done in parallel
     */
    void compute( const Unit deltaUnit, Crew *para );
    
private:
    Unit   delta; // for parallel coding
    Kernel pCode; // the parallel code
    YOCTO_DISABLE_COPY_AND_ASSIGN(PHI);
    void paraCompute(Context &);
    void prepareMixed();
};

#endif

