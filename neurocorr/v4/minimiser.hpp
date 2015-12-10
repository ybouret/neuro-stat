#ifndef NC_MINIMISER_INCLUDED
#define NC_MINIMISER_INCLUDED 1


#include "types.hpp"
#include "yocto/sequence/many-arrays.hpp"
#include "yocto/counted-object.hpp"
#include "yocto/math/types.hpp"

typedef many_arrays<Real, memory::global> ManyArrays;

//! one minimiser, for one thread
class Minimiser : public counted_object
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
    size_t                 count;
    
    //! allocate memory
    explicit Minimiser(const matrix_of<Real> &usrG);

    virtual ~Minimiser() throw();

    //! prepare b,d, and g for neurone i
    template <typename T>
    inline void prepare(const matrix_of<T> &mu1,
                        const matrix_of<T> &mu2,
                        const matrix_of<T> &muA,
                        const size_t        i,
                        const Real          gam)
    {
        assert(i>0);
        assert(i<=mu2.cols);

        assert(n==mu2.rows);
        assert(n==muA.rows);

        const Real glnp = gam*lnp;
        const Real vfac = 2*glnp;
        const Real afac = glnp/3;
        for(size_t r=1;r<=n;++r)
        {
            b[r] = mu1(r,i);
            const Real v = mu2(r,i);
            const Real A = muA(r,1);
            d[r] = math::Sqrt(vfac*v)+afac*A;
            g[r] = G(r,r);
            if(g[r]<=0) g[r] = 1;
        }
    }


    void update();

    Real compute_H() const throw();

    void run();

private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(Minimiser);

};


#include "yocto/ptr/arc.hpp"
class Minimisers
{
public:
    typedef arc_ptr<Minimiser> MinPtr;

    virtual  ~Minimisers() throw();
    explicit  Minimisers(const size_t           dim,
                         const size_t           neurones,
                         const Real             usrGam,
                         threading::crew       *team);

    const size_t   num;

    void  run(threading::crew *team);


private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(Minimisers);
    void compute( const threading::context &ctx ) throw();

    vector<MinPtr>         mpv;

public:
    matrix<Real>           G;
    matrix<Real>           a;
    matrix<Real>           mu1;
    matrix<Real>           mu2;
    matrix<Real>           muA;
    vector<Real>           count;
    Real                   gam;

private:
    threading::sequential_executor kSeq;


};

#endif
