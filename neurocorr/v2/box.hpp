#ifndef NC_BOX_INCLUDED
#define NC_BOX_INCLUDED 1

#include "types.hpp"
#include "phifn.hpp"
#include "yocto/associative/map.hpp"

class   Box;
typedef slots_of<Box> _Boxes;


class Box
{
public:
    typedef int              Kind;
    typedef map<Kind,size_t> KindDB;

    const   size_t   trial;     //!< associated trial
    const   Unit     tauStart;  //!< tau start
    const   Unit     tauFinal;  //!< tau final
    const   Kind     kind;      //!< label
    mutable size_t   indx;      //!< index for collecting data

    explicit Box(const size_t indx,
                 const Unit   ini,
                 const Unit   end,
                 const Kind   aka=0) throw();
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

    inline friend std::ostream & operator<<(std::ostream &os, const Box &b )
    {
        os << "[@" << b.trial << ":" << b.tauStart << "->" << b.tauFinal << "|$" << b.kind << "]";
        return os;
    }

private:
    YOCTO_DISABLE_ASSIGN(Box);
};


#endif
