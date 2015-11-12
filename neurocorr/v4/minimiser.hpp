#ifndef NC_MINIMISER_INCLUDED
#define NC_MINIMISER_INCLUDED 1


#include "types.hpp"
#include "yocto/sequence/many-arrays.hpp"


typedef many_arrays<Real, memory::global> ManyArrays;

class Minimiser
{
public:
    const matrix<Real> &G;
    const size_t        n; //!< the dimension
    ManyArrays          arrays;
    array<Real>        &b;
    array<Real>        &v;
    array<Real>        &d;
    const Real          lnp;
    Real                gam;

    //! allocate memory
    explicit Minimiser(const matrix<Real> &usrG);
    virtual ~Minimiser() throw();

    void find(const Real         usr_gam,
              const array<Real> &usr_b,
              const array<Real> &usr_v,
              const array<Real> &usr_a);


private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(Minimiser);

};

#endif
