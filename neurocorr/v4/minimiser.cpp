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
s( n ),
arrays(8),
b(  arrays.next_array() ),
d(  arrays.next_array() ),
a(  arrays.next_array() ),
y(  arrays.next_array() ),
q(  arrays.next_array() ),
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

size_t Minimiser:: update()
{
    size_t nch = 0;
    for(size_t i=n;i>0;--i)
    {
        // local constant
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
            if(s[i]!=1)
            {
                s[i] = 1;
                ++nch;
            }
            y[i] = a[i] -tmp;
            continue;
        }

        if(Di<-di)
        {
            const Real tmp = a[i];
            a[i] = (Di+di)/G[i][i];
            if(s[i]!=-1)
            {
                s[i]=-1;
                ++nch;
            }
            y[i] = a[i] - tmp;
            continue;
        }

        if(s[i]!=0)
        {
            s[i] = 0;
            ++nch;
        }
        y[i] = -a[i];
        a[i] = 0;
    }
    return nch;

}

size_t Minimiser:: update_v2()
{
    size_t nch = 0;
    for(size_t i=n;i>0;--i)
    {
        // local constant
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
            switch(s[i])
            {
                case -1:
                    ++nch;
                    s[i] = 0;
                    a[i] = 0;
                    break;

                case 0:
                    ++nch;
                    s[i] = 1;
                    break;

                default: assert(1==s[i]);
                    break;
            }
            y[i] = a[i] -tmp;
            continue;
        }

        if(Di<-di)
        {
            const Real tmp = a[i];
            a[i] = (Di+di)/G[i][i];
            switch(s[i])
            {
                case 1:
                    ++nch;
                    s[i] = 0;
                    a[i] = 0;
                    break;

                case 0:
                    ++nch;
                    s[i] = -1;
                    break;

                default:
                    assert(-1==s[i]);
                    break;
            }
            y[i] = a[i] - tmp;
            continue;
        }

        if(s[i]!=0)
        {
            s[i] = 0;
            ++nch;
        }
        y[i] = -a[i];
        a[i] = 0;
    }
    return nch;
    
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

Real Minimiser:: compute_H() const throw()
{
    Real H = 0;

    for(size_t i=n;i>0;--i)
    {
        const Real ai = a[i];
        H += d[i] * Fabs( ai ) - b[i] * ai;
    }
    H += tao::quadratic(G,a);
    return H;
}




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

void Minimiser::compute_q() const throw()
{
    tao::mul(q,G,a);
    tao::subp(q,b);
    for(size_t i=n;i>0;--i)
    {
        if(s[i]!=0)
        {
            q[i] -= s[i] * d[i];
        }
        else
        {
            if(q[i]>d[i])
            {
                q[i] -= d[i];
            }
            else
            {
                if(q[i]<-d[i])
                {
                    q[i] += d[i];
                }
                else
                {
                    q[i] = 0;
                }
            }
        }
    }
}

#include "yocto/ios/ocstream.hpp"

void Minimiser:: run()
{

    ios::wcstream fp("err.dat");
    tao::ld(a,0);
    tao::ld(s,0);
    size_t count = 0;
    fp("0 %g 0\n", compute_H() );
    while(true)
    {
        ++count;
        const size_t nch = update();
        std::cerr << "nch=" << nch << std::endl;
        fp("%u %g %d\n", unsigned(count), compute_H(), int(nch) );
        if(count>=50)
            break;
    }
}

void Minimiser:: run2()
{

    ios::wcstream fp("err2.dat");
    tao::ld(a,0);
    tao::ld(s,0);
    size_t count = 0;
    fp("0 %g 0\n", compute_H() );
    while(true)
    {
        ++count;
        const size_t nch = update();
        compute_q();
        std::cerr << "a=" << a << std::endl;
        std::cerr << "s=" << s << std::endl;
        std::cerr << "y=" << y << std::endl;
        std::cerr << "q=" << q << std::endl;

        const Real qsq = tao::norm_sq(q);
        if(qsq>0)
        {
            const Real asq = tao::norm_sq(a);
            tao::mulby(Sqrt(asq/qsq),q);
            std::cerr << "beta=" << q << std::endl;
        }

        std::cerr << "nch=" << nch << std::endl;
        fp("%u %g %d\n", unsigned(count), compute_H(), int(nch) );
        if(count>=50)
            break;
    }
}




