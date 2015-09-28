#ifndef NC_BOX_INCLUDED
#define NC_BOX_INCLUDED 1

#include "types.hpp"
#include "phifn.hpp"

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

    //! compute B
    /**
     B.rows = 1+Phi.K*Phi.neurones
     B.cols = Phi.neurones
     The use of Tau must be different in case of parallel or sequential code
     */
    void computeRHS(const PHI &Phi, Matrix<Unit> &B,UList &tau) const;

    //! compute G
    /**
     G.rows = G.cols = 1+Phi.K*Phi.N
     */
    void computeMATRIX(const PHI &Phi, Matrix<Unit> &G ) const;


private:
    YOCTO_DISABLE_ASSIGN(Box);
};

#endif
