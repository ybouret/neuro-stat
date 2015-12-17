#include "minimiser.hpp"
//#include "yocto/math/opt/bracket.hpp"
//#include "yocto/math/opt/minimize.hpp"
#include "yocto/math/core/tao.hpp"

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

Minimiser:: Minimiser(const matrix<Real> &usrG,
                      const matrix<Real> &usrQ) :
G(usrG),
dim( check_dims(G) ),
Q(usrQ),
arrays(6),
b(  arrays.next_array() ),
d(  arrays.next_array() ),
a(  arrays.next_array() ),
g(  arrays.next_array() ),
s(  arrays.next_array() ),
p(  arrays.next_array() ),
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
        s[i] = a[i];
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

        {
            Real tmp = 0;
            const array<Real> &Gi = G[i];
            for(size_t j=1;j<i;++j)
            {
                tmp += Gi[j] * a[j];
            }
            p += tmp * ai;
        }
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
        a[i] = s[i] = p[i] = 0;
    }

    if(false)
    {
        for(size_t i=dim;i>0;--i)
        {
            Real ans = 0;
            const array<Real> &Qi = Q[i];
            for(size_t j=dim;j>0;--j)
            {
                ans += Qi[j] * b[j];
            }
            a[i] = ans;
            p[i] = a[i];
        }
    }



    count        = 0;
    Real   H_old = compute_H();

#if SAVE_H == 1
    const string filename = vformat( "H%u.dat", unsigned(neurone) );
    ios::wcstream fp(filename);
    fp("0 %.16lf %.15e\n", H_old, compute_err() );
#endif
    while(true)
    {
        ++count;
        update();
        const Real H_new = compute_H();
#if SAVE_H == 1
        fp("%u %.16lf %.15e\n", unsigned(count), H_new, compute_err());
#endif
        const Real dH = H_old - H_new;
        if(dH<=0)
        {
#if SAVE_H == 1
            fp("#reached minimum\n");
            for(size_t i=0;i<10;++i)
            {
                update();
                fp("%u %.16lf %.15e\n", unsigned(++count), compute_H(), compute_err());
            }
#endif
            break;
        }
        else
        {
            // save better point
            tao::set(p,a);
        }
        H_old = H_new;
    }

    // restore best point
    tao::set(a,p);
    final = compute_H();
}



Real Minimiser:: update2(Real H_old)
{
    Real H_new = H_old;
    for(size_t i=dim;i>0;--i)
    {
        const array<Real> &Gi = G[i];
        s[i] = a[i];
        Real Di = 0;
        for(size_t j=dim;j>0;--j)
        {
            if(i!=j)
            {
                Di += Gi[j] * a[j];
            }
        }
        Di = b[i] - Di;
        const Real di = d[i];

        if(Di>di)
        {
            a[i] = (Di-di)/g[i];
            goto CHECK;
        }

        if(Di<-di)
        {
            a[i] = (Di+di)/g[i];
            goto CHECK;
        }

        a[i] = 0;
    CHECK:
        H_new = compute_H();
        if(H_new < H_old )
        {
            //__________________________________________________________________
            //
            // accept new a[i], register new H value
            //__________________________________________________________________
            H_old = H_new;
            continue;
        }
        else
        {
            if(H_new>H_old)
            {
                //______________________________________________________________
                //
                // reject new a[i], keep old H value
                //______________________________________________________________
                a[i] = s[i];
                continue;
            }
            else
            {
                //______________________________________________________________
                //
                // numerical equality: keep the coordinate
                // with the smallest amplitude,
                // and keep numerical smallest H value
                //______________________________________________________________
                if(Fabs(s[i])<Fabs(a[i]))
                {
                    a[i] = s[i];
                }
                H_old = min_of(H_old, H_new);
                continue;
            }
        }
    }

    return H_new;
}




void Minimiser:: run2()
{
    //__________________________________________________________________________
    //
    // Initialize
    //__________________________________________________________________________
    for(size_t i=dim;i>0;--i)
    {
        s[i] = a[i] = 0;
    }
    count = 0;
    Real H_org = compute_H();

#if SAVE_H == 1
    const string filename = vformat( "newH%u.dat", unsigned(neurone) );
    ios::wcstream fp(filename);
    fp("0 %.16lf %.15e\n", H_org, compute_err() );
#endif
    while(true)
    {
        ++count;
        const Real H_new = update2(H_org);
#if SAVE_H == 1
        fp("%u %.16lf %.15e\n", unsigned(count), H_new, compute_err());
#endif
        const Real dH = H_org - H_new;
        if(dH<=0)
        {
            break;
        }
        H_org = H_new;
    }
    final = H_org;
}


Real Minimiser:: compute_err() const throw()
{
    Real ans = 0;
    for(size_t i=dim;i>0;--i)
    {
        const Real tmp = Fabs(a[i]-s[i]);
        if(tmp>ans)
        {
            ans = tmp;
        }
    }
    return ans;
}


////////////////////////////////////////////////////////////////////////////////
//
// using minimises in parallel
//
////////////////////////////////////////////////////////////////////////////////
Minimisers:: ~Minimisers() throw()
{
}

Minimisers:: Minimisers(const matrix<Real>    &usrG,
                        const matrix<Real>    &usrQ,
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
        const MinPtr p( new Minimiser(usrG,usrQ) );
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

void Minimisers:: compute2( const threading::context &ctx ) throw()
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
        opt.run2();
        for(size_t r=a.rows;r>0;--r)
        {
            a(r,i) = opt.a[r];
        }
        count[i] = opt.count;
        H[i]     = opt.final;
    }
}


void  Minimisers:: run2(threading::crew *team)
{
    threading::kernel_executor &kExec = team ? *static_cast<threading::kernel_executor*>(team) : kSeq;
    threading::kernel           kMini(this, & Minimisers::compute2);
    
    kExec(kMini);
}





