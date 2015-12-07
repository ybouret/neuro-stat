#ifndef NC_MINIMISER_INCLUDED
#define NC_MINIMISER_INCLUDED 1


#include "types.hpp"
#include "yocto/sequence/many-arrays.hpp"


typedef many_arrays<Real, memory::global> ManyArrays;

class Minimiser
{
public:
    const matrix_of<Real> &G;
    const size_t           n; //!< the dimension
    ManyArrays             arrays;
    array<Real>           &b;
    array<Real>           &d;
    array<Real>           &a;
    array<Real>           &g;
    const Real             lnp;
    
    //! allocate memory
    explicit Minimiser(const matrix_of<Real> &usrG);

    virtual ~Minimiser() throw();

    //! prepare b,d, and g
    void prepare(const matrix<Unit> &mu1,
                 const matrix<Unit> &mu2,
                 const matrix<Unit> &muA,
                 const size_t        i,
                 const Real          gam);


    void update();

    Real compute_H() const throw();

    void run();

private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(Minimiser);

};

#endif
