#ifndef NC_BOXES_INCLUDED
#define NC_BOXES_INCLUDED 1

#include "box.hpp"
#include "yocto/sequence/addr-list.hpp"

//! to group boxes by same trial
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

    void buildDB( Box::KindDB &db ) const;

private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(Boxes);
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
