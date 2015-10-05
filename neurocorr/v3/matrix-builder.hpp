#ifndef NC_MATRIX_BUILDER_INCLUDED
#define NC_MATRIX_BUILDER_INCLUDED 1


#include "boxes.hpp"
#include "yocto/sequence/addr-list.hpp"

typedef addr_list<const Box> BoxList;
typedef addr_node<const Box> BoxNode;
//YOCTO_SUPPORT_BITWISE_OPS(BoxList);

//#include "yocto/sequence/vector.hpp"

class MatrixBuilder
{
public:
    ~MatrixBuilder() throw();
    MatrixBuilder(Matrices<Unit> &matrices,
                  const Boxes    &boxes,
                  const PHI      &usrPhi,
                  Crew           *para);

private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(MatrixBuilder);
    Matrices<Unit>   &MG;
    const PHI        &Phi;
    slots_of<BoxList> mgr;
};


#endif

