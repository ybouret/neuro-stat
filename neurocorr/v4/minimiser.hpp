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
    const matrix<Real> &G;       //!< reference to the shared G matrix
    const size_t        dim;     //!< the dimension
    const matrix<Real> &Q;       //!< pseudo inverse for G
    ManyArrays          arrays;  //!< local memory
    array<Real>        &b;       //!< copy of b vector
    array<Real>        &d;       //!< d vector, MUST be prepared
    array<Real>        &a;       //!< current set of variables
    array<Real>        &g;       //!< diagonal part of G
    array<Real>        &s;       //!< saving last parameters;
    array<Real>        &p;       //!< best point
    size_t              count;   //!< #iteration for last run
    Real                final;   //!< final value
    size_t              neurone; //!< last processed neurone

    //! allocate memory
    explicit Minimiser(const matrix<Real> &usrG,
                       const matrix<Real> &usrQ);

    //! release all memory
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

        assert(dim==mu2.rows);
        assert(dim==muA.rows);

        const size_t neurones= mu1.cols;
        const size_t params  = dim*neurones;
        const Real   lnp     = math::Log( Real(params) );
        const Real   glnp    = gam*lnp;
        const Real   vfac    = 2*glnp;
        const Real   afac    = glnp/3;
        for(size_t r=1;r<=dim;++r)
        {
            b[r] = mu1(r,i);
            const Real v = mu2(r,i);
            const Real A = muA(r,1);
            d[r] = math::Sqrt(vfac*v)+afac*A;
            g[r] = G(r,r);
            if(g[r]<=0) g[r] = 1;
        }

        neurone = i;
    }


    void update();
    Real compute_H() const throw();
    void run();

    //! max(|a[i]-s[i]|)
    Real compute_err() const throw();

    Real update2(Real Horg);
    void run2();


private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(Minimiser);

};


#include "yocto/ptr/arc.hpp"
class Minimisers
{
public:
    typedef arc_ptr<Minimiser> MinPtr;

    virtual  ~Minimisers() throw();
    explicit  Minimisers(const matrix<Real>    &usrG,
                         const matrix<Real>    &usrQ,
                         const matrix_of<Real> &usrMu1,
                         const matrix_of<Real> &usrMu2,
                         const matrix_of<Real> &usrMuA,
                         matrix_of<Real>       &usrA,
                         array<Real>           &usrCnt,
                         array<Real>           &usrH,
                         const Real             usrGam,
                         threading::crew       *team);

    const size_t   num; //!< #threads

    void  run(threading::crew *team);
    
    void  run2(threading::crew *team);


private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(Minimisers);
    void compute( const threading::context &ctx ) throw();
    void compute2( const threading::context &ctx ) throw();

    vector<MinPtr>         mpv;

public:
    const matrix_of<Real> &mu1;
    const matrix_of<Real> &mu2;
    const matrix_of<Real> &muA;
    matrix_of<Real>       &a;
    array<Real>           &count;
    array<Real>           &H;
    Real                   gam;

private:
    threading::sequential_executor kSeq;


};

#endif
