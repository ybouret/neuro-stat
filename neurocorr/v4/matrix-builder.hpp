#ifndef NC_MATRIX_BUILDER_INCLUDED
#define NC_MATRIX_BUILDER_INCLUDED 1


#include "phi.hpp"
#include "boxes.hpp"
#include "symindex.hpp"

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
	YOCTO_DISABLE_COPY_AND_ASSIGN(MatrixBuilder);
    typedef addr_list<const Box> BoxList;
    typedef addr_node<const Box> BoxNode;
    
    matrices_of<Unit> &MG;
    const PHI         &Phi;
    const Box         *box;
    size_t             JJ;
    SymIndex           S;
    vector<BoxList>    mgr;
    vector<CPW>        prod;

    void computeSide( threading::context &ctx );
    void computeCore( threading::context &ctx );
};

#endif
