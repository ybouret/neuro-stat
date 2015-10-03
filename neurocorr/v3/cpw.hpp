#ifndef NC_CPW_INCLUDED
#define NC_CPW_INCLUDED 1

#include "train.hpp"
#include "yocto/container/tuple.hpp"

YOCTO_PAIR_DECL(coord,const Unit,tau,Unit,value);
YOCTO_DISABLE_ASSIGN(coord);
public:
static inline
int compare(const coord &lhs,const coord&rhs) throw()
{
    return lhs.tau - rhs.tau;
}
YOCTO_PAIR_END();

typedef dynamic_slots<coord> _CPW;


YOCTO_TRIPLE_DECL(Moments,Unit,mu1,Unit,mu2,Unit,muA);
YOCTO_DISABLE_ASSIGN(Moments);
public:
inline Moments() throw() : mu1(0), mu2(0), muA(0) {}
void   reset()   throw() { mu1=mu2=muA=0; }
inline friend bool operator==(const Moments &lhs,const Moments &rhs) throw()
{
    return (lhs.mu1 == rhs.mu1) && (lhs.mu1==rhs.mu2) && (lhs.muA==rhs.muA);
}
inline friend bool operator!=(const Moments &lhs,const Moments &rhs) throw()
{
    return (lhs.mu1!=rhs.mu1) || (lhs.mu2!=rhs.mu2) || (lhs.muA!=rhs.muA);
}

YOCTO_TRIPLE_END();

//! warning, assuming sorted coordinates
class CPW : public object, public _CPW
{
public:
    Unit foot;
    
    virtual ~CPW() throw();
    explicit CPW(size_t max_points=0);
    
    void buildFrom(const RArray<Unit> &spikes, const Unit delta); //!< build function
    void shiftFrom(const CPW &other, const Unit delta);           //!< shift tau's
    void productOf(const CPW &lhs, const CPW &rhs);               //!< product of others
    
    void sort() throw(); //!< sort if necessary
    void save(const char *filename) const;
    void insert(const Unit tau, const Unit value) throw();
    
    //! single evaluator
    Unit operator()(const Unit tau) const throw();
    
    //! multiple evaluations
    void evalSumOn(const Train &tr,
                   const size_t length,
                   const size_t offset,
                   Moments     &moments) const throw();
    
    //! multiple evaluations, naive version
    void evalSumOn_(const Train &tr,
                    const size_t length,
                    const size_t offset,
                    Moments     &moments) const throw();
    
private:
    YOCTO_DISABLE_ASSIGN(CPW);
};

#endif

