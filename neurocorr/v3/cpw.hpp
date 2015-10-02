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

//! warning, assuming sorted coordinates
class CPW : public object, public CVector<coord>
{
public:
    Unit foot;

    virtual ~CPW() throw();
    explicit CPW(size_t max_points);

    void sort() throw(); //!< sort if necessary

    void save(const char *filename) const;

private:
    YOCTO_DISABLE_ASSIGN(CPW);
};

#endif

