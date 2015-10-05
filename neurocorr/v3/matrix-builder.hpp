#ifndef NC_MATRIX_BUILDER_INCLUDED
#define NC_MATRIX_BUILDER_INCLUDED 1


#include "boxes.hpp"
#include "yocto/sequence/addr-list.hpp"

typedef addr_list<const Box> BoxList;
typedef addr_node<const Box> BoxNode;

class MatrixBuilder
{
public:
    ~MatrixBuilder() throw();
    MatrixBuilder(Matrices<Unit> &matrices,
                  const Boxes    &boxes,
                  const PHI      &usrPhi,
                  Crew           *para);

    YOCTO_TRIPLE_DECL(Task,const size_t,i,const size_t,k,const size_t,I);
    YOCTO_DISABLE_ASSIGN(Task);
    YOCTO_TRIPLE_END();

private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(MatrixBuilder);
    const Box        *box; // shared box for linear computation
    size_t            J;   // shared trial index for mixed computation
    Matrices<Unit>   &MG;
    const PHI        &Phi;
    slots_of<BoxList> mgr;
    slots_of<Task>    tasks;

    void computeSide(Context &ctx);
    void computeCore(Context &ctx);
    
};


#endif

