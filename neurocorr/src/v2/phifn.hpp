#ifndef NC_PHIFN_INCLUDED
#define NC_PHIFN_INCLUDED 1


#include "records.hpp"
#include "cpwfn.hpp"
#include "yocto/threading/crew.hpp"

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

typedef threading::crew Crew;
typedef Crew::kernel    Kernel;
typedef Crew::context   Context;

class PHI : public PHI_Base
{
public:
    const size_t &trials;
    const size_t &neurons;
    explicit PHI(const size_t extra, const Records &records);
    virtual ~PHI() throw();

    //! compute the set of Phi functions for a given delta
    void compute( const Unit deltaUnit, Crew *para );

    
    
private:
    Unit   delta; // for parallel coding
    Kernel pCode; // the parallel code
    YOCTO_DISABLE_COPY_AND_ASSIGN(PHI);
    void paraCompute(Context &);
};

#endif

