#ifndef NC_BOXES_MIXED_INCLUDED
#define NC_BOXES_MIXED_INCLUDED 1

#include "boxes.hpp"

class MixedEvaluator
{
public:
    explicit MixedEvaluator(const Boxes       &boxes,
                            const PHI         &UsrPhi,
                            Matrices          &Gmatrices,
                            Crew              *para);
    
    virtual ~MixedEvaluator() throw();
    
private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(MixedEvaluator);
    const PHI              &Phi;
    const size_t           trials; //!< Phi.trials
    size_t                 J;      //!< trial index
    slots_of<BoxList>      mgr;    //!< box, ordered by trials
    slots_of<CPW_Function> prod;   //!< memory for product function
    Matrices              &G;
    void compute(Context &ctx);
};


#endif

