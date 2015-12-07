#include "minimiser.hpp"
#include "yocto/math/opt/bracket.hpp"
#include "yocto/math/opt/minimize.hpp"

using namespace math;

Minimiser:: ~Minimiser() throw()
{

}

static inline size_t check_dims(const matrix_of<Real> &G)
{
    if(G.cols!=G.rows)
        throw exception("Minimiser: expecting square matrix");

    if(G.rows<=1)
        throw exception("Minimiser: not enough rows/cols");

    return G.rows;
}

Minimiser:: Minimiser(const matrix_of<Real> &usrG) :
G(usrG),
n( check_dims(G) ),
arrays(4),
b(  arrays.next_array() ),
d(  arrays.next_array() ),
a(  arrays.next_array() ),
g(  arrays.next_array() ),
lnp( log(Real(n)) )
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
        g[r] = G(r,r);
        if(g[r]<=0) g[r] = 1;
    }
    std::cerr << "b=" << b << std::endl;
    std::cerr << "d=" << d << std::endl;

}

void Minimiser:: update()
{
    for(size_t i=n;i>0;--i)
    {
        Real Di = 0;
        for(size_t j=n;j>0;--j)
        {
            if(i!=j)
            {
                Di += G(i,j) * a[j];
            }
        }
        Di = b[i] - Di;
        const Real di = d[i];

        if(Di>di)
        {
            a[i] = (Di-di)/g[i];
            continue;
        }

        if(Di<-di)
        {
            a[i] = (Di+di)/g[i];
            continue;
        }

        a[i] = 0;
    }
}



Real Minimiser:: compute_H() const throw()
{
    Real H = 0;
    Real q = 0;
    Real p = 0;
    for(size_t i=n;i>0;--i)
    {
        const Real ai = a[i];
        H += d[i] * Fabs( ai ) - b[i] * ai;
        q += G(i,i) * ai*ai;
        for(size_t j=1;j<i;++j)
        {
            p += G(i,j) * a[j] * ai;
        }
    }

    H += H;
    H += q+(p+p);
    return H;
}




#include "yocto/ios/ocstream.hpp"

void Minimiser:: run()
{

    for(size_t i=n;i>0;--i)
    {
        a[i] = 0;
    }
    size_t count = 0;
    Real   H_old = compute_H();

    ios::wcstream fp("err.dat");
    fp("0 %g\n", H_old );
    while(true)
    {
        ++count;
        update();
        const Real H_new = compute_H();
        fp("%u %g\n", unsigned(count), H_new );
        const Real dH = H_old - H_new;
        //std::cerr << "H=" << H_new << ", dH=" << dH << std::endl;
        if(dH<=0)
            break;
        H_old = H_new;
    }
    
}




