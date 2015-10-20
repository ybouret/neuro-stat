#ifndef NC_CPW_INCLUDED
#define NC_CPW_INCLUDED 1

#include "train.hpp"
#include "yocto/container/tuple.hpp"

YOCTO_PAIR_DECL(coord,const Unit,tau,const Unit,value);
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

YOCTO_PAIR_DECL(Moments,Unit,mu1,Unit,mu2);
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

typedef vector<coord> _CPW;

class CPW : public _CPW
{
public:
    Unit foot;
    explicit CPW() throw();
    explicit CPW(size_t n);
    virtual ~CPW() throw();
    CPW(const CPW &);

    void add(const Unit tau,const Unit value);
    void save(const char *filename) const;
    void save_sample(const char *filename) const;
    friend bool operator==(const CPW &lhs, const CPW &rhs) throw();
    friend bool operator!=(const CPW &lhs, const CPW &rhs) throw();

    //! thread safe if memory preallocated
    void buildFrom(const UArray &train, const Unit delta, UVector &shift);

    //! access
    Unit operator()( const Unit tau ) const throw();

    //! evaluating moments
    void evalMoments_(const UArray &tau,
                      const size_t start,
                      const size_t count,
                      Moments     &moments) const throw();

    void evalMoments(const UArray &tau,
                     const size_t start,
                     const size_t count,
                     Moments     &moments) const throw();
private:
    YOCTO_DISABLE_ASSIGN(CPW);
};


#endif
