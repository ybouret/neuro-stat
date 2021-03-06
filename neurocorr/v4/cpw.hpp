#ifndef NC_CPW_INCLUDED
#define NC_CPW_INCLUDED 1

#include "train.hpp"
#include "yocto/container/tuple.hpp"

//! coordinates for a Constant PieceWise function
YOCTO_PAIR_DECL(YOCTO_TUPLE_STANDARD,coord,const Unit,tau,const Unit,value);
friend inline bool operator==(const coord &lhs, const coord &rhs) throw()
{
    return (lhs.tau == rhs.tau) && (lhs.value==rhs.value);
}
friend inline bool operator!=(const coord &lhs, const coord &rhs) throw()
{
    return (lhs.tau != rhs.tau) || (lhs.value!=rhs.value);
}
YOCTO_PAIR_END();
YOCTO_SUPPORT_C_STYLE_OPS(coord);


//! moments collector for computation
YOCTO_PAIR_DECL(YOCTO_TUPLE_STANDARD,Moments,Unit,mu1,Unit,mu2);
inline Moments() throw() : mu1(0), mu2(0) {}
inline friend bool operator==(const Moments &lhs,const Moments &rhs) throw()
{
    return lhs.mu1==rhs.mu1 && lhs.mu2==rhs.mu2;
}
inline friend bool operator!=(const Moments &lhs, const Moments &rhs) throw()
{
    return lhs.mu1!=rhs.mu1 || lhs.mu2!=rhs.mu2;
}
YOCTO_PAIR_END();


//! base class for CPW
typedef vector<coord> _CPW;

//! a vector of ordered coordinates
class CPW : public _CPW
{
public:
    Unit     foot;
    explicit CPW() throw();
    explicit CPW(size_t n);
    virtual ~CPW() throw();
    CPW(const CPW &);

    void   add(const Unit tau,const Unit value); //!< append and check
    void __add(const Unit tau,const Unit value); //!< append, no check

    //! save the contour
    void save(const char *filename) const;

    //! save the evaluated points by the access operator
    void save_sample(const char *filename) const;

    //! test functions equality, for testing
    friend bool operator==(const CPW &lhs, const CPW &rhs) throw();

    //! test functions difference, for testing
    friend bool operator!=(const CPW &lhs, const CPW &rhs) throw();

    //! thread safe if memory preallocated
    void buildFrom(const UArray &train, const Unit delta, UVector &shift);

    //! thread save if memory is preallocated
    void product(const CPW &lhs, const CPW &rhs);

    //! access
    Unit operator()( const Unit tau ) const throw();

    //! evaluating moments
    void evalMoments_(const UArray &tau,
                      const size_t start,
                      const size_t count,
                      Moments     &moments) const throw();

    //! evaluating moments, optimised
    void evalMoments(const UArray &tau,
                     const size_t start,
                     const size_t count,
                     Moments     &moments) const throw();

    //! get max of function, naive function
    Unit maxAbsOn_(const Unit tauStart, const Unit tauFinal) const throw();

    //! get max of function, optimised version
    Unit maxAbsOn( const Unit tauStart, const Unit tauFinal) const throw();

    //! integrate, naive function
    Unit integrate_(const Unit tauStart, const Unit tauFinal) const throw();

    //! integrate, optimized function
    Unit integrate( Unit tauStart,  Unit tauFinal) const throw();

private:
    YOCTO_DISABLE_ASSIGN(CPW);
};


#endif
