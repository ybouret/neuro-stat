#ifndef NC_BOXES_INTG_INCLUDED
#define NC_BOXES_INTG_INCLUDED 1

#include "boxes.hpp"
#include "yocto/ptr/auto-arr.hpp"

//! evaluate first row/col of G
/**
 use Phi functions
 */
class IntegralEvaluator
{
public:
    explicit IntegralEvaluator(const Boxes    &boxes,
                               const PHI      &usrPhi,
                               Matrices       &Gmatrices,
                               Crew           *para);

    virtual ~IntegralEvaluator() throw();

    YOCTO_QUAD_DECL(Task,const Box *,box,size_t,neurone,size_t,k,size_t,I);
    YOCTO_DISABLE_ASSIGN(Task);
    YOCTO_QUAD_END();


private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(IntegralEvaluator);
    const PHI     &Phi;
    Matrices      &G;
    slots_of<Task> Tasks;

    void compute(Context &ctx);
};



#endif

