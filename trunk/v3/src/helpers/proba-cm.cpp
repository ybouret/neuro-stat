#include "yocto/sequence/vector.hpp"
#include "yocto/fs/vfs.hpp"
#include "yocto/exception.hpp"
#include "yocto/math/kernel/matrix.hpp"
#include "yocto/ios/icstream.hpp"
#include "yocto/string/conv.hpp"
#include "yocto/associative/map.hpp"
#include "yocto/mpa/rational.hpp"
#include "yocto/sort/quick.hpp"
#include "yocto/code/combination.hpp"

using namespace yocto;
using namespace math;

typedef ptrdiff_t              count_t;
typedef vector<count_t>        ivector_t;
typedef matrix<count_t>        imatrix_t;
typedef map<count_t,count_t>   imap_t;
typedef vector<mpq>            qvector_t;



static inline
bool get_next_line( string &line, ios::istream &fp )
{
    line.clear();
    return fp.read_line(line) >= 0;
}

static inline
bool load_matrix( imatrix_t &M, ios::istream &fp )
{
    string line;
    if(!get_next_line(line, fp) ) return false;
    const size_t rows = strconv::to<size_t>(line,"#rows");
    if(!get_next_line(line,fp)) throw exception("missing #cols");
    const size_t cols = strconv::to<size_t>(line,"#cols");
    
    M.make(rows,cols);
    for(size_t i=1;i<=rows;++i)
    {
        for(size_t j=1;j<=rows;++j)
        {
            if(!get_next_line(line, fp))
                throw exception("Missing matrix[%u][%u]", unsigned(i), unsigned(j));
            M[i][j] = count_t(strconv::to<double>( line, "matrix element"));
        }
    }
    
    return true;
}


class Context
{
public:
    
    explicit Context() :
    A(),
    M(0),
    lambda(),
    occurs(),
    nu(0),
    ratio(),
    kappa(),
    lmin(0),
    lmax(0),
    cmax(0),
    pdf(),
    cdf(),
    socks(0),
    drawers(0),
    meta(0),
    walls(0),
    wall(),
    factM()
    {}
    
    virtual ~Context() throw() {}
    
    imatrix_t      A;         //!< matrix of coincidences
    size_t         M;         //!< matrix size
    ivector_t      lambda;    //!< all the different #coincidences
    ivector_t      occurs;    //!< #occurences of lambda
    size_t         nu;         //!< lambda and occurs size: number of classes
    qvector_t      ratio;     //!< single proba
    vector<size_t> kappa;     //!< indices to build proba
    count_t        lmin;
    count_t        lmax;
    count_t        cmax;
    qvector_t      pdf;
    qvector_t      cdf;
    size_t         socks;
    size_t         drawers;
    size_t         meta;
    size_t         walls;
    vector<size_t> wall;
    mpn            factM;
    vector<double> rcdf;
    
    bool get_next( ios::istream &fp )
    {
        if( load_matrix(A,fp) )
        {
            build_classes();
            compute_proba();
            return true;
        }
        else
            return false;
    }
    
private:
    
    //__________________________________________________________________________
    //
    // build all useful information to perform the computation
    //__________________________________________________________________________
    
    inline void build_classes()
    {
        M = A.rows;
        lambda.free();
        occurs.free();
        ratio.free();
        kappa.free();
        imap_t dict;
        
        for(size_t i=1;i<=M;++i)
        {
            for(size_t j=1;j<=M;++j)
            {
                if(i!=j)
                {
                    const count_t k = A[i][j];
                    count_t *pNum = dict.search(k);
                    if(!pNum)
                    {
                        if(!dict.insert(k,1))
                            throw exception("unexpected insert failure");
                    }
                    else
                    {
                        assert(*pNum>0);
                        ++(*pNum);
                    }
                }
            }
        }
        nu = dict.size();
        lambda.ensure(nu);
        occurs.ensure(nu);
        ratio.ensure(nu);
        kappa.ensure(nu);
        factM = mpn::factorial(M);
        
        for( imap_t::iterator i = dict.begin();i!=dict.end();++i)
        {
            const count_t k = i->key;
            lambda.push_back(k);
            occurs.push_back(*i);
        }
        co_qsort(lambda, occurs);
        const size_t __den = M * (M-1);
        for(size_t i=1;i<=nu;++i)
        {
            const mpq p(occurs[i],__den);
            ratio.push_back(p);
        }
        
        mpq sum;
        for(size_t i=1;i<=nu;++i)
        {
            sum += ratio[i];
        }
        
        if(sum.den != 1 || sum.num != 1)
        {
            throw exception("unexpected sum of ratio failure");
        }
        
        std::cerr << "M       = " << M      << std::endl;
        std::cerr << "nu      = " << nu     << std::endl;
        std::cerr << "lambda  = " << lambda << std::endl;
        std::cerr << "occurs  = " << occurs << std::endl;
        std::cerr << "ratio   = " << ratio  << std::endl;
        
        lmin = lambda[1];
        lmax = lambda[nu];
        cmax = lmax * M;
        std::cerr << "lambda_min = " << lmin << std::endl;
        std::cerr << "lambda_max = " << lmax << std::endl;
        std::cerr << "max_coinc  = " << cmax << std::endl;
        mpq zero;
        pdf.make(cmax+1,zero);
        cdf.make(cmax+1,zero);
    }
    
    //__________________________________________________________________________
    //
    // combinatorics...
    //__________________________________________________________________________
    inline void compute_proba()
    {
        kappa.make(nu,0);
        socks   = M;
        drawers = nu;
        meta    = socks + drawers - 1;
        walls   = drawers-1;
        const mpn      combi   = mpn::binomial(meta,walls);
        std::cerr << "building " << combi << " combinations" << std::endl;
        wall.make(walls,0);
        combination C(meta,walls);
        
        mpn nk;
        do
        {
            gen_kappa(C,nk);
            update_proba();
        }
        while( C.next() );
        
        std::cerr << std::endl;
        std::cerr << "generated " << nk << "/" << combi << std::endl;
        std::cerr << "pdf=" << pdf << std::endl;
        cdf[1] = pdf[1];
        for(size_t i=2;i<=pdf.size();++i)
        {
            cdf[i] = cdf[i-1] + pdf[i];
        }
        std::cerr << "cdf=" << cdf << std::endl;
        const mpq &sum = cdf.back();
        if( !(sum.num==1&&sum.den==1) )
            throw exception("error in probabilities!");
        
        rcdf.make(cdf.size(), 0);
        for(size_t i=1;i<=cdf.size();++i)
        {
            rcdf[i] = cdf[i].to_double();
        }
        std::cerr << "rcdf=" << rcdf << std::endl;
    }
    
    
    inline void gen_kappa( const combination &C, mpn &nk)
    {
        for(size_t j=0,i=1;i<=walls;++i,++j)
        {
            wall[i] = C[j]+1;
        }
        kappa[1] = wall[1]-1;
        for(size_t i=2,j=1;i<=walls;++i,++j)
        {
            kappa[i] = (wall[i] - wall[j])-1;
        }
        kappa[nu]  = meta - wall[walls];
        size_t sum = 0;
        for(size_t i=1;i<=nu;++i)
        {
            sum += kappa[i];
        }
        if(sum!=M)
            throw exception("invalid combination!");
        ++nk;
        std::cerr << "kappa=" << kappa << " #" << nk << "  \r";
        std::cerr.flush();
    }
    
    inline void update_proba()
    {
        size_t ncoinc = 0;
       
        mpq    proba  = factM;
        for(size_t i=1;i<=nu;++i)
        {
            ncoinc += lambda[i] * kappa[i];
            proba *= mpq::power(ratio[i], kappa[i]);
            proba /= mpn::factorial( kappa[i] );
        }
        
        pdf[ncoinc+1] += proba;
    }
    
};



int main(int argc, char *argv[] )
{
    const char *progname = vfs::get_base_name(argv[0]);
    try
    {
        Context ctx;
        for(int iarg=1;iarg<argc;++iarg)
        {
            const string  fn = argv[iarg];
            ios::icstream fp(fn);
            while( ctx.get_next(fp) )
            {
            }
        }
        
        return 0;
    }
    catch(...)
    {
        std::cerr << "unhandle exception in " << progname << std::endl;
    }
    return -1;
}
