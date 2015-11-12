#include "minimiser.hpp"
#include "yocto/math/opt/bracket.hpp"
#include "yocto/math/opt/minimize.hpp"

Minimiser:: ~Minimiser() throw()
{

}

static inline size_t check_dims(const matrix<Real> &G)
{
    if(G.cols!=G.rows)
        throw exception("Minimiser: expecting square matrix");

    if(G.rows<=1)
        throw exception("Minimiser: not enough rows/cols");

    return G.rows;
}

Minimiser:: Minimiser(const matrix<Real> &usrG) :
G(usrG),
n( check_dims(G) ),
arrays(8),
b(    arrays.next_array() ),
v(    arrays.next_array() ),
d(    arrays.next_array() ),
aorg( arrays.next_array() ),
atry( arrays.next_array() ),
beta( arrays.next_array() ),
lnp( log(Real(n)) ),
gam(0)
{
    arrays.allocate(n);
}


#include "yocto/math/core/tao.hpp"
using namespace math;

void Minimiser:: find(const Real         usr_gam,
                      const array<Real> &usr_b,
                      const array<Real> &usr_v,
                      const array<Real> &usr_a)
{
    assert(usr_b.size()>=n);
    assert(usr_v.size()>=n);
    assert(usr_gam>0);

    gam = usr_gam;
    const double glp  = gam*lnp;
    const double vfac = 2.0 * glp;
    const double afac = glp/3.0;

    for(size_t i=1;i<=n;++i)
    {
        const double bi = usr_b[i];
        const double vi = usr_v[i];
        const double ai = usr_a[i];

        assert(bi>=0);
        assert(vi>=0);
        assert(ai>=0);

        b[i] = bi;
        v[i] = vi;
        d[i] = sqrt(vfac*vi)+afac*fabs(ai);
    }
    std::cerr << "b=" << b << std::endl;
    std::cerr << "v=" << v << std::endl;
    std::cerr << "d=" << d << std::endl;

    tao::ld(aorg, 0);

    for(size_t iter=0;iter<20;++iter)
    {
        std::cerr << "aorg=" << aorg << std::endl;
        Beta(aorg);
        //std::cerr << "beta=" << beta << std::endl;
        std::cerr << "H="    << H(aorg) << std::endl;
        numeric<Real>::function F(this, & Minimiser::Eval );
        triplet<Real> xx = { 0, 1, 0 };
        triplet<Real> ff = { F(0), F(1), 0 };
        //std::cerr << "start xx=" << xx << ", ff=" << ff << std::endl;
        bracket<Real>::expand(F, xx, ff);
        //std::cerr << "final xx=" << xx << ", ff=" << ff << std::endl;
        minimize(F, xx, ff, 0.0);
        const Real lam = xx.b;
        for(size_t i=n;i>0;--i)
        {
            aorg[i] = aorg[i] - lam * beta[i];
        }
    }

}

int  Minimiser:: SignOf(const Real x) throw()
{
    if(x<0)
    {
        return -1;
    }
    else
    {
        if(x>0)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
}

Real Minimiser::H(const array<Real> &a) const throw()
{
    assert(a.size()>=n);
    Real ab=0;
    Real ad=0;
    for(size_t i=n;i>0;--i)
    {
        const Real ai = a[i];
        ab += b[i] * ai;
        ad += d[i] * ai * SignOf(ai);
    }
    const Real del = ad-ab;
    return (del+del)+tao::quadratic(G,a);
}

Real Minimiser:: Eval(const Real x)  throw()
{
    tao::setprobe(atry, aorg, -x, beta);
    return H(atry);
}


void Minimiser:: Beta(const array<Real> &a) const throw()
{
    tao::mul(beta, G, a);
    for(size_t i=n;i>0;--i)
    {
        const Real ai = a[i];
        beta[i] += (SignOf(ai)*d[i]-b[i]);
    }
}
