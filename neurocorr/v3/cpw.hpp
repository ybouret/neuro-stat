#ifndef NC_CPW_INCLUDED
#define NC_CPW_INCLUDED 1

#include "types.hpp"
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

//! warning, assuming sorted coordinates
class CPW : public object, public _CPW
{
public:
    Unit foot;

    virtual ~CPW() throw();
    explicit CPW(size_t max_points=0);
    
    void buildFrom(const RArray<Unit> &spikes, const Unit delta);


    void sort() throw(); //!< sort if necessary
    void save(const char *filename) const;
    void insert(const Unit tau, const Unit value) throw();


private:
    YOCTO_DISABLE_ASSIGN(CPW);
};

#endif

