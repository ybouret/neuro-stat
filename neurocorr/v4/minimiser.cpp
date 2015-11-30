#include "minimiser.hpp"
#include "yocto/math/opt/bracket.hpp"
#include "yocto/math/opt/minimize.hpp"

using namespace math;

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

Minimiser:: Minimiser(const matrix<Real> &usrG, const Real ftol) :
G(usrG),
n( check_dims(G) ),
arrays(8),
b(  arrays.next_array() ),
d(  arrays.next_array() ),
a(  arrays.next_array() ),
y(  arrays.next_array() ),
D(  arrays.next_array() ),
q(  arrays.next_array() ),
atry( arrays.next_array() ),
s(  n ),
lnp( log(Real(n)) ),
eps( Fabs(ftol) )
{
    arrays.allocate(n);
}

#include "yocto/code/utils.hpp"

void Minimiser:: prepare(const matrix<Unit> &mu1,
                         const matrix<Unit> &mu2,
                         const matrix<Unit> &muA,
                         const size_t        i,
                         const Real          gam)
{
    assert(i>0);
    assert(i<=mu2.cols);

    assert(n==mu2.rows);
    assert(n==muA.rows);

    const Real glnp = gam*lnp;
    const Real vfac = 2*glnp;
    const Real afac = glnp/3.0;
    for(size_t r=1;r<=n;++r)
    {
        b[r] = mu1[r][i];
        const Real v = mu2[r][i];
        const Real A = muA[r][1];
        d[r] = Sqrt(vfac*v)+afac*A;
    }
    std::cerr << "b=" << b << std::endl;
    std::cerr << "d=" << d << std::endl;

}

#include "yocto/math/core/tao.hpp"

void Minimiser:: update()
{
    for(size_t i=n;i>0;--i)
    {
        Real Di = 0;
        for(size_t j=n;j>0;--j)
        {
            if(i!=j)
            {
                Di += G[i][j] * a[j];
            }
        }
        Di = b[i] - Di;
        const Real di = d[i];

        if(Di>di)
        {
            const Real tmp = a[i];
            a[i] = (Di-di)/G[i][i];
            y[i] = a[i] -tmp;
            continue;
        }

        if(Di<-di)
        {
            const Real tmp = a[i];
            a[i] = (Di+di)/G[i][i];
            y[i] = a[i] - tmp;
            continue;
        }

        y[i] = -a[i];
        a[i] = 0;
    }

}

bool Minimiser:: converged() const throw()
{
    for(size_t i=n;i>0;--i)
    {
        if( Fabs(y[i]) > Fabs(eps*a[i]) )
            return false;
    }
    return true;
}


void Minimiser:: sink()
{
    size_t nsink = 0;
SINK_LOOP:
    for(size_t i=n;i>0;--i)
    {
        Real Di = 0;
        for(size_t j=n;j>0;--j)
        {
            if(i!=j)
            {
                Di += G[i][j] * a[j];
            }
        }
        Di = b[i] - Di;

        if(Fabs(Di)<=d[i])
        {
            if(s[i]!=0)
            {
                a[i] = 0;
                s[i] = 0;
                ++nsink;
            }
        }
    }

    std::cerr << "nsink=" << nsink << std::endl;
    if(nsink>0)
    {
        nsink=0;
        goto SINK_LOOP;
    }
}




#include "yocto/sort/quick.hpp"
Real Minimiser:: compute_error() const throw()
{
    Real ans = 0.0;
    for(size_t i=1;i<=n;++i)
    {
        const Real tmp = y[i];
        ans += tmp*tmp;
    }
    return Sqrt(ans);
}

void Minimiser:: update_v2()
{
    // estimator to see who might be zero
    for(size_t i=n;i>0;--i)
    {
        Real tmp = 0;
        for(size_t j=n;j>0;--j)
        {
            if(i!=j)
            {
                tmp += G[i][j] * a[j];
            }
        }
        D[i] = b[i] - tmp;
    }

    std::cerr << "D=" << D << std::endl;

    // predicted
    for(size_t i=n;i>0;--i)
    {
        const Real Di = D[i];
        const Real di = d[i];

        if(Di>di)
        {
            a[i] = (Di-di)/G[i][i];
            s[i] = 1;
            continue;
        }

        if(Di<-di)
        {
            a[i] = (Di+di)/G[i][i];
            s[i] = -1;
            continue;
        }
        
        s[i] = 0;
        a[i] = 0;
    }
    
    
}

void Minimiser:: compute_q()
{
    // q0 = b - Ga
    tao::mul(q,G,a);
    tao::subp(q,b);
    std::cerr << "qraw=" << q << std::endl;
    for(size_t i=n;i>0;--i)
    {
        switch(s[i])
        {
            case 1:
                q[i] -= d[i];
                break;

            case -1:
                q[i] += d[i];
                break;

            case 0:

                //break;

            default:
                throw exception("invalid sign[%u]=%d", unsigned(i), s[i]);
        }
    }
    std::cerr << "q=" << q << std::endl;
    const Real qq = tao::norm(q);
    const Real aa = tao::norm(a);
    std::cerr << "aa=" << aa << std::endl;
    std::cerr << "qq=" << qq << std::endl;
    tao::mulby(aa/qq,q);
}


void Minimiser::forward()
{
    numeric<Real>::function F( this, & Minimiser::H );

    triplet<Real> ZZ = { 0, 1, 1 };
    std::cerr << "ZZ=" << ZZ << std::endl;
    triplet<Real> FF = { F(0), F(1), 0 };
    FF.c = FF.b;
    std::cerr << "FF=" << FF << std::endl;
    bracket<Real>::expand(F, ZZ, FF);
    std::cerr << "ZZ=" << ZZ << std::endl;
    std::cerr << "FF=" << FF << std::endl;
    minimize(F, ZZ, FF, eps);
    const Real zmin = ZZ.b;
    const Real Hmin = F(zmin);
    std::cerr << "atry=" << atry << std::endl;
    std::cerr << "Hmin=" << Hmin << std::endl;
    tao::set(y,a);
    tao::set(a,atry);
    update_v2();
    std::cerr << "afin=" << a << std::endl;
    std::cerr << "sfin=" << s << std::endl;

}

Real Minimiser:: H(Real z) const
{
    tao::setprobe(atry,a, z, q);
    Real value = 0;
    for(size_t i=n;i>0;--i)
    {
        value += d[i] * Fabs(atry[i]) - b[i] * atry[i];
    }
    value += value;
    value += tao::quadratic(G,atry);
    return value;
}

