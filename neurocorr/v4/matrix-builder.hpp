#ifndef NC_MATRIX_BUILDER_INCLUDED
#define NC_MATRIX_BUILDER_INCLUDED 1


#include "phi.hpp"
#include "boxes.hpp"

#include "yocto/sequence/addr-list.hpp"

class MatrixBuilder
{
public:
    virtual ~MatrixBuilder() throw();
    explicit MatrixBuilder(matrices_of<Unit> &usrMG,
                           const Boxes       &boxes,
                           const PHI         &usrPhi,
                           threading::crew  *team);


private:
    typedef addr_list<const Box> BoxList;

    matrices_of<Unit> &MG;
    const PHI         &Phi;
    const Box         *box;
    vector<BoxList>    mgr;


    void computeSide( threading::context &ctx );
};

#endif
