#ifndef NC_VECTOR_BUILDER_INCLUDED
#define NC_VECTOR_BUILDER_INCLUDED 1


#include "boxes.hpp"

class VectorBuilder
{
public:
    ~VectorBuilder() throw();

    //! build matrices of vectors
    /**
     The number of matrices depends on the
     desired grouping.
     Their sizes are Phi.dim * Phi.neurones.
     Matrices must be initialized before call.

     \param mu1      matrices of count,sum Phi...
     \param mu2      matrices of count,sum Phi^2
     \param muA      COLUMN matrices of 1, max(|Phi|)
     \param boxes    boxes with precomputed indices
     \param phi      precomputed phi functions with a given delta
     
     The parallelization occurs only on neurones
     */
    VectorBuilder(Matrices<Unit> &mu1,
                  Matrices<Unit> &mu2,
                  Matrices<Unit> &muA,
                  const Boxes    &boxes,
                  const PHI      &usrPhi,
                  Crew           *para);

private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(VectorBuilder);
    void compute(Context &ctx);
    void computeA(Context &ctx);

    Matrices<Unit> &Mu1;
    Matrices<Unit> &Mu2;
    Matrices<Unit> &MuA;
    const PHI      &Phi;
    const Box      *box;

};

#endif
