#include "yocto/sequence/vector.hpp"
#include "yocto/fs/vfs.hpp"
#include "yocto/exception.hpp"
#include "yocto/math/kernel/matrix.hpp"
#include "yocto/ios/icstream.hpp"
#include "yocto/string/conv.hpp"
#include "yocto/ordered/sorted-vector.hpp"
#include "yocto/associative/map.hpp"
#include "yocto/mpa/rational.hpp"

using namespace yocto;
using namespace math;

typedef ptrdiff_t              count_t;
typedef vector<count_t>        ivector_t;
typedef matrix<count_t>        imatrix_t;
typedef sorted_vector<count_t> isorted_t;
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

static inline
void build_classes(isorted_t       &lam,
                   ivector_t       &num,
                   const imatrix_t &M )
{
    lam.free();
    num.free();
    imap_t dict;
    
    for(size_t i=1;i<=M.rows;++i)
    {
        for(size_t j=1;j<=M.rows;++j)
        {
            if(i!=j)
            {
                const count_t k = M[i][j];
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
    const size_t nmax = dict.size();
    lam.ensure(nmax);
    num.ensure(nmax);
    for( imap_t::iterator i = dict.begin();i!=dict.end();++i)
    {
        const count_t k = i->key;
        if(!lam.insert(k))
            throw exception("unexpected insert failure level-2");
        num.push_back(*i);
    }
    std::cerr << "lam=" << lam << std::endl;
    std::cerr << "num=" << num << std::endl;
    
}

class Context
{
public:
    
    explicit Context() {}
    virtual ~Context() throw() {}
    
    imatrix_t A;    //!< matrix of coincidences
    size_t    M;    //!< matrix size
    isorted_t lam;  //!< all the different #coincidences
    ivector_t num;   //!< #occurences of lambda
    size_t    nmax;  //!< lam/num size
    qvector_t prob;  //!< single proba
    
    bool get_next( ios::istream &fp )
    {
        if( load_matrix(A,fp) )
        {
            M = A.rows;
            build_classes(lam, num, A);
            nmax = lam.size();
            prob.free();
            prob.ensure(nmax);
            const size_t __den = M * (M-1);
            for(size_t i=1;i<=nmax;++i)
            {
                const mpq p(num[i],__den);
                prob.push_back(p);
            }
            std::cerr << "prob=" << prob << std::endl;
            mpq sum;
            for(size_t i=1;i<=nmax;++i)
            {
                sum += prob[i];
            }
            return true;
        }
        else
            return false;
        
    }
    
private:
    
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
