#ifndef NC_BOX_INCLUDED
#define NC_BOX_INCLUDED 1

#include "types.hpp"
#include "phifn.hpp"


class   Box;
typedef slots_of<Box> _Boxes;

class Box
{
public:
    const size_t   trial;     //!< associated trial
    const Unit     tauStart;  //!< tau start
    const Unit     tauFinal;  //!< tau final
    const int      kind;      //!< label

    explicit Box(const size_t indx,
                 const Unit   ini,
                 const Unit   end) throw();
    virtual ~Box() throw();
    Box(const Box &) throw();


    //! extract the sorted list of times within the box in the train...
    void extract( UList &Tau, const Train &train  ) const;

    //! append values to a B vector
    /**
     B.rows = 1+Phi.K*Phi.neurones
     B.cols = Phi.neurones
     The use of Tau must be different in case of parallel or sequential code
     */
    void appendRHS(const PHI &Phi, Matrix<Unit> &B,UList &tau) const;

    //! compute G
    /**
     G.rows = G.cols = 1+Phi.K*Phi.N
     */
    void computeMATRIX(const PHI &Phi, Matrix<Unit> &G ) const;

    void appendLinearTo(Matrix<Unit> &G,const PHI &Phi) const;

#if 0
    void appendMixedTo(Matrix<Unit>       &G,
                       const CPW_Function &F,
                       const Mix          &g);
#endif


private:
    YOCTO_DISABLE_ASSIGN(Box);
};


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

    void updateMixed(Matrix<Unit>  *regG,
                     const size_t   numG,
                     const PHI     &Phi,
                     Crew          *para);

private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(Boxes);
    dynamic_slots<CPW_Function> prod;
    void allocateProducts(const size_t count,const size_t np);
};

#endif
