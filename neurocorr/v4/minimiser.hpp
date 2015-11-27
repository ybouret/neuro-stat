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
    array<Real>        &d;
    array<Real>        &a;
    array<Real>        &y;
    array<Real>        &D;
    const Real          lnp;
    Real                gam;

    //! allocate memory
    explicit Minimiser(const matrix<Real> &usrG);

    virtual ~Minimiser() throw();

    void prepare(const matrix<Unit> &mu1,
                 const matrix<Unit> &mu2,
                 const matrix<Unit> &muA,
                 const size_t        i);


    void update();
    void update_v2();

private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(Minimiser);

};

#endif
