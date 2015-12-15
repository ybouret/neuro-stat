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

Minimiser:: Minimiser(const matrix_of<Real> &usrG,
                      const matrix_of<Real> &usrQ) :
G(usrG),
dim( check_dims(G) ),
Q(usrQ),
arrays(4),
b(  arrays.next_array() ),
d(  arrays.next_array() ),
a(  arrays.next_array() ),
g(  arrays.next_array() ),
count(0),
final(0),
neurone(0)
{
    if(Q.cols!=dim||Q.rows!=dim) throw exception("Minimiser: invalid G pseudo inverse size");
    arrays.allocate(dim);
}



void Minimiser:: update()
{
    for(size_t i=dim;i>0;--i)
    {
        Real Di = 0;
        for(size_t j=dim;j>0;--j)
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
    for(size_t i=dim;i>0;--i)
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


#define SAVE_H 1

#if SAVE_H == 1
#include "yocto/ios/ocstream.hpp"
#endif

void Minimiser:: run()
{

    for(size_t i=dim;i>0;--i)
    {
        a[i] = 0;
    }

    if(true)
    {
        for(size_t i=dim;i>0;--i)
        {
            Real ans = 0;
            for(size_t j=dim;j>0;--j)
            {
                ans += Q(i,j) * b[j];
            }
            a[i] = ans;
        }
    }

    count        = 0;
    Real   H_old = compute_H();

#if SAVE_H == 1
    const string filename = vformat( "H%u.dat", unsigned(neurone) );
    ios::wcstream fp(filename);
    fp("0 %g\n", H_old );
#endif
    while(true)
    {
        ++count;
        update();
        const Real H_new = compute_H();
#if SAVE_H == 1
        fp("%u %g\n", unsigned(count), H_new );
#endif
        const Real dH = H_old - H_new;
        if(dH<=0)
            break;
        H_old = H_new;
    }
    final = compute_H();
}


////////////////////////////////////////////////////////////////////////////////
//
// using minimises in parallel
//
////////////////////////////////////////////////////////////////////////////////
Minimisers:: ~Minimisers() throw()
{
}

Minimisers:: Minimisers(const matrix_of<Real> &usrG,
                        const matrix_of<Real> &usrQ,
                        const matrix_of<Real> &usrMu1,
                        const matrix_of<Real> &usrMu2,
                        const matrix_of<Real> &usrMuA,
                        matrix_of<Real>       &usrA,
                        array<Real>           &usrCnt,
                        array<Real>           &usrH,
                        const Real             usrGam,
                        threading::crew       *team) :
num( team ? team->size : 1),
mpv(num,as_capacity),
G(usrG),
Q(usrQ),
mu1(usrMu1),
mu2(usrMu2),
muA(usrMuA),
a(usrA),
count(usrCnt),
H(usrH),
gam(usrGam)
{

    for(size_t i=1;i<=num;++i)
    {
        const MinPtr p( new Minimiser(G,Q) );
        mpv.push_back(p);
    }
}


void Minimisers:: compute( const threading::context &ctx ) throw()
{
    // get the corresponding minimiser
    Minimiser   &opt      = *mpv[ctx.indx];

    // get the number of columns to process
    const size_t neurones = mu1.cols;
    size_t       i        = 1;
    size_t       length   = neurones;
    ctx.split(i, length);

    // process each column
    for(;length>0;++i,--length)
    {
        opt.prepare(mu1, mu2, muA, i, gam);
        opt.run();
        for(size_t r=a.rows;r>0;--r)
        {
            a(r,i) = opt.a[r];
        }
        count[i] = opt.count;
        H[i]     = opt.final;
    }
}

void  Minimisers:: run(threading::crew *team)
{
    threading::kernel_executor &kExec = team ? *static_cast<threading::kernel_executor*>(team) : kSeq;
    threading::kernel           kMini(this, & Minimisers::compute);

    kExec(kMini);

    
}


