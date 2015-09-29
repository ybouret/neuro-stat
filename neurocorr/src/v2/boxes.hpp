#ifndef NC_BOXES_INCLUDED
#define NC_BOXES_INCLUDED 1

#include "box.hpp"

#include "yocto/sequence/addr-list.hpp"

typedef addr_list<Box> BoxList;
YOCTO_SUPPORT_BITWISE_OPS(BoxList);

//! Boxes
class Boxes : public Object, public _Boxes
{
public:
    //! extracted from a R matrix
    /**
     \warning trials indices start from 1 in R data
     */
    explicit Boxes( const Real usr_scale, const Matrix<Real> &RBoxes);

    //just reserve memory
    explicit Boxes( const Real usr_scale, const size_t nboxes);

    virtual ~Boxes() throw();

    // update the rhs vectors
    void updateLinearVectors(const PHI &Phi);

    void updateMixed(const PHI     &Phi,
                     Crew          *para);

private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(Boxes);
    typedef vector<BoxList>             BoxManager;
    typedef dynamic_slots<CPW_Function> Products;
    typedef addr_node<Box>              BoxNode;

    size_t                         J; //!< current trial...
    const PHI *                 pPHI; //!< current PHI functions

    //__________________________________________________________________________
    //
    // data for linear terms evaluation
    //__________________________________________________________________________


    //__________________________________________________________________________
    //
    // data for mixed terms evaluation
    //__________________________________________________________________________
    BoxManager                  bmgr; //!<
    Products                    prod; //!< store the current(s) function
    Kernel                      kMix; //!< call evalMixed

    void mapBoxesPerTrial(const size_t trials);
    void allocateProducts(const size_t count,const size_t np);
    void evalMixed(Context &ctx);
};

class MixedEvaluator
{
public:
    explicit MixedEvaluator(Boxes &boxes,const PHI &UsrPhi, Crew *para);
    virtual ~MixedEvaluator() throw();

private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(MixedEvaluator);
    const PHI              &Phi;
    const size_t           trials; //!< Phi.trials
    size_t                 J;      //!< trial index
    slots_of<BoxList>      mgr;    //!< box, ordered by trials
    slots_of<CPW_Function> prod;   //!< memory for product function
    Kernel                 run;    //!< call compute
    void compute(Context &ctx);
};

#endif
