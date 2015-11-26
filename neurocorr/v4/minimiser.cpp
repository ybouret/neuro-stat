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
b( arrays.next_array() ),
d( arrays.next_array() ),
a( arrays.next_array() ),
y( arrays.next_array() ),
D( arrays.next_array() ),
lnp( log(Real(n)) ),
gam(0)
{
    arrays.allocate(n);
}

#include "yocto/code/utils.hpp"
using namespace math;

void Minimiser:: prepare(const matrix<Unit> &mu1,
                         const matrix<Unit> &mu2,
                         const matrix<Unit> &muA,
                         const size_t        i)
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
        //______________________________________________________________________
        //
        // Compute D
        //______________________________________________________________________
        Real &Di = D[i];
        Di = 0;
        for(size_t j=n;j>0;--j)
        {
            if(i!=j)
            {
                Di += G[i][j] * a[j];
            }
        }
        Di = b[i] - Di;
#if defined(FULL) && FULL==1
    }

    std::cerr << "D=" << D << std::endl;

    for(size_t i=n;i>0;--i)
    {
        const Real Di = D[i];
#endif
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

