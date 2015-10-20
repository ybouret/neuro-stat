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


private:
    YOCTO_DISABLE_ASSIGN(CPW);
};


#endif
