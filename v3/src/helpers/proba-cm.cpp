#include "yocto/sequence/vector.hpp"
#include "yocto/fs/vfs.hpp"
#include "yocto/exception.hpp"
#include "yocto/math/kernel/matrix.hpp"
#include "yocto/ios/icstream.hpp"
#include "yocto/string/conv.hpp"
#include "yocto/associative/map.hpp"
#include "yocto/mpa/rational.hpp"
#include "yocto/sort/quick.hpp"

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
    lam(),
    num(),
    nmax(0),
    ratio(),
    lmin(0),
    lmax(0),
    cmax(0)
    {}
    
    virtual ~Context() throw() {}
    
    imatrix_t A;      //!< matrix of coincidences
    size_t    M;      //!< matrix size
    ivector_t lam;    //!< all the different #coincidences
    ivector_t num;    //!< #occurences of lambda
    size_t    nmax;   //!< lam/num size
    qvector_t ratio;  //!< single proba
    count_t   lmin;
    count_t   lmax;
    count_t   cmax;
    
    bool get_next( ios::istream &fp )
    {
        if( load_matrix(A,fp) )
        {
            build_classes();
            return true;
        }
        else
            return false;
        
    }
    
private:
    inline void build_classes()
    {
        M = A.rows;
        lam.free();
        num.free();
        ratio.free();
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
        nmax = dict.size();
        lam.ensure(nmax);
        num.ensure(nmax);
        ratio.ensure(nmax);
        
        for( imap_t::iterator i = dict.begin();i!=dict.end();++i)
        {
            const count_t k = i->key;
            lam.push_back(k);
            num.push_back(*i);
        }
        co_qsort(lam, num);
        const size_t __den = M * (M-1);
        for(size_t i=1;i<=nmax;++i)
        {
            const mpq p(num[i],__den);
            ratio.push_back(p);
        }
        
        mpq sum;
        for(size_t i=1;i<=nmax;++i)
        {
            sum += ratio[i];
        }
        if(sum.den != 1 || sum.num != 1)
        {
            throw exception("unexpected sum of ratio failure");
        }
        
        
        std::cerr << "lam=" << lam << std::endl;
        std::cerr << "num="  << num << std::endl;
        std::cerr << "ratio=" << ratio << std::endl;

        lmin = lam[1];
        lmax = lam[nmax];
        cmax = lmax * M;
        std::cerr << "lambda_min=" << lmin << std::endl;
        std::cerr << "lambda_max=" << lmax << std::endl;
        std::cerr << "max_coinc =" << cmax << std::endl;
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
