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


    //!
    void extract( UList &Tau, const Train &train  ) const;

    //! compute B: TODO store in vector ?
    void computeRHS(const PHI &Phi, Matrix<Unit> &B,UList &tau) const;

    //! compute G..
    void computeMat(const PHI &Phi) const;


private:
    YOCTO_DISABLE_ASSIGN(Box);
};

#endif
