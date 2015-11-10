#ifndef NC_VECTOR_BUILDER_INCLUDED
#define NC_VECTOR_BUILDER_INCLUDED 1

#include "phi.hpp"
#include "boxes.hpp"


YOCTO_TRIPLE_DECL(inside,size_t,count,size_t,start,const UArray *,train);
inline inside() throw() : count(0), start(0), train(0) {}
YOCTO_DISABLE_ASSIGN(inside);
YOCTO_PAIR_END();

class VectorBuilder
{
public:
    virtual ~VectorBuilder() throw();

    //! add content to matrices
    /**
     The number of matrices must match the Boxes::assignIndices count.

     \param usrMu1 (1+N*K) x N matrices of first moments
     \param usrMu2 (1+N*K) x N matrices of first monments
     \param usrMuA (1+N*K) x 1 matrices of max absolute values
     \param boxes  the boxes where to compute
     \param usrPhi the precomputed K Phi functions
     \param team   for parallel computation
     
     for each box, a parallel loop will process a portion
     of the N*K terms.

     */
    explicit VectorBuilder(matrices_of<Unit> &usrMu1,
                           matrices_of<Unit> &usrMu2,
                           matrices_of<Unit> &usrMuA,
                           const Boxes       &boxes,
                           const PHI         &usrPhi,
                           threading::crew  *team);

private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(VectorBuilder);

    matrices_of<Unit> &Mu1;
    matrices_of<Unit> &Mu2;
    matrices_of<Unit> &MuA;
    const PHI         &Phi;
    vector<inside>     ins;
    const Box         *box;


    void compute( threading::context &ctx );

};

#endif

