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
        q += g[i] * ai*ai;
        Real tmp = 0;
        for(size_t j=1;j<i;++j)
        {
            tmp += G(i,j) * a[j];
        }
        p += tmp * ai;
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
    std::cerr << "#count=" << count << std::endl;
    
}


////////////////////////////////////////////////////////////////////////////////
Minimisers:: ~Minimisers() throw()
{
}

Minimisers:: Minimisers(const matrix_of<Real> &usrG, threading::crew *team) :
num( team ? team->size : 1),
mp(num,as_capacity)
{
    for(size_t i=1;i<=num;++i)
    {
        const MinPtr p( new Minimiser(usrG) );
        mp.push_back(p);
    }
}

