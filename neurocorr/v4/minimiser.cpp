#include "minimiser.hpp"

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
arrays(4),
b( arrays.next_array() ),
v( arrays.next_array() ),
d( arrays.next_array() ),
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

}


