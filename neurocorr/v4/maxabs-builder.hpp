#ifndef NC_MAXABS_BUILDER_INCLUDED
#define NC_MAXABS_BUILDER_INCLUDED 1

#include "boxes.hpp"
#include "phi.hpp"

class MaxAbsBuilder
{
public:
    //! build the 1 colum matrix of max abs
    /**
     
     parallelize on each N*K Phi function
     */
    explicit MaxAbsBuilder(matrices_of<Unit> &usrMuA,
                           const Boxes       &boxes,
                           const PHI         &usrPhi,
                           threading::crew   *team
                           );
    virtual ~MaxAbsBuilder() throw();



private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(MaxAbsBuilder);
    matrices_of<Unit> &MuA;
    const PHI         &Phi;
    const Box         *box;

    void compute(threading::context &ctx);

};


#endif
