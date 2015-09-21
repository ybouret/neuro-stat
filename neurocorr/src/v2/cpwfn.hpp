#ifndef NC_CPWFN_INCLUDED
#define NC_CPWFN_INCLUDED 1


#include "train.hpp"
#include "yocto/container/tuple.hpp"
#include "yocto/bitwise.hpp"
#include "yocto/sequence/vector.hpp"

YOCTO_PAIR_DECL(Coord,Unit,tau,Unit,value);
YOCTO_PAIR_END();
YOCTO_SUPPORT_C_STYLE_OPS(Coord);

class CPW_Function : public Object
{
public:
    //! a function with an intial cpacity
    explicit CPW_Function(const Real usr_scale, const size_t n=0);
    virtual ~CPW_Function() throw();

    Unit foot; //!< value before first point

    void insert(const Unit tau, const Unit value);
    size_t size() const throw();
    const Coord & operator[](const size_t indx) const throw();
    const Coord & front() const throw();
    const Coord & back() const throw();

    void buildFrom( const RArray<Unit> &train, const Unit deltaUnit );

    void saveTo(const char *filename) const;

private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(CPW_Function);
    vector<Coord> coords;
};

#endif
