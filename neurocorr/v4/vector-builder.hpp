#ifndef NC_VECTOR_BUILDER_INCLUDED
#define NC_VECTOR_BUILDER_INCLUDED 1

#include "phi.hpp"
#include "boxes.hpp"

YOCTO_TRIPLE_DECL(inside,size_t,count,size_t,start,const UArray *,arr);
inline inside() throw() : count(0), start(0), arr(0) {}
YOCTO_PAIR_END();

class VectorBuilder
{
public:
    virtual ~VectorBuilder() throw();

    //! add content to matrices
    /**

     */
    explicit VectorBuilder(matrices_of<Unit> &usrMu1,
                           matrices_of<Unit> &usrMu2,
                           const Boxes       &boxes,
                           const PHI         &usrPhi,
                           threading::crew  *team);

private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(VectorBuilder);

    matrices_of<Unit> &Mu1;
    matrices_of<Unit> &Mu2;
    const PHI         &Phi;
    vector<inside>     ins;
    const Box         *box;


    void compute( threading::context &ctx );

};

#endif

