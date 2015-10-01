#ifndef NC_BOXES_LINEAR_INCLUDED
#define NC_BOXES_LINEAR_INCLUDED 1

#include "boxes.hpp"

class LinearEvaluator
{
public:
    explicit LinearEvaluator(const Boxes &boxes,
                             const PHI   &UsrPhi,
                             Matrices    &Bmatrices,
                             Crew        *para);
    
    virtual ~LinearEvaluator() throw();
    
private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(LinearEvaluator);
    const Box   *box; //!<current box
    const PHI   &Phi; //!<functions
    Matrices    &B;   //!<matrices to fill, (1+N*K) x (N)
    slots_of<UList> TauList;
    const size_t   &count;
    
    void compute(Context &ctx);
    
};

#endif

