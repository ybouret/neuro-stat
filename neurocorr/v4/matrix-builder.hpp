#ifndef NC_MATRIX_BUILDER_INCLUDED
#define NC_MATRIX_BUILDER_INCLUDED 1


#include "phi.hpp"
#include "boxes.hpp"
#include "symindex.hpp"

#include "yocto/sequence/addr-list.hpp"

//! build the 'G' matrices
class MatrixBuilder
{
public:
    virtual ~MatrixBuilder() throw();

    //! construct the G matrices
    /**
     The number of matrices must match the Boxes::assignIndices count.
     \param usrMG  the G matrices, (1+N*K)*(1+N*K)
     \param boxes  the boxes where to compute the terms
     \param usrPhi the pre-computed phi matrices
     \param team   for parallel code
     
     a first loop compute the (N*K) side in parallel,
     and register the boxes per trial.
     The parallelism occurs on the (N*K) terms

     a second loop is performed over the existing trials,
     so that each of the product is computed only once.
     
     The parallelism occurs on the (N*K)(1+N*K)/2 terms,
     using a SymIndex class to compute the row/col term to compute.

     */
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
