#include "yocto/fs/vfs.hpp"
#include "yocto/ios/ocstream.hpp"
#include "yocto/ios/icstream.hpp"
#include "yocto/string/conv.hpp"
#include "yocto/exception.hpp"
#include "yocto/math/kernel/matrix.hpp"
#include "yocto/mpa/mpi.hpp"

#include "yocto/mpa/rational.hpp"
#include "yocto/associative/map.hpp"
#include "yocto/sort/quick.hpp"
#include "yocto/container/tab2d.hpp"

#include "yocto/counting/dispatch.hpp"
#include "yocto/core/mpi-split.hpp"
#include "yocto/eta.hpp"
#include "yocto/duration.hpp"

using namespace yocto;
using namespace math;

typedef ptrdiff_t            count_t;
typedef matrix<count_t>      imatrix_t;
typedef vector<count_t>      ivector_t;
typedef vector<mpq>          qvector_t;
typedef map<count_t,count_t> imap_t;

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
void eta_callback( const mpi &MPI, const eta &ETA )
{
    const duration done(ETA.time_done);
    const duration left(ETA.time_left);
    const double   pc = ETA.ratio_done * 100.0;
    MPI.Printf0(stderr, "%7.2f%% | %02uD%02uH%02uM%02.0fs | %02uD%02uH%02uM%02.0fs  \r",
                pc,
                done.d, done.h, done.m, done.s,
                left.d, left.h, left.m, left.s);
    
}

int main(int argc, char *argv[] )
{
    const char *progname = vfs::get_base_name(argv[0]);
    try
    {
        YOCTO_MPI(SINGLE);
        MPI.CloseStdIO();
        MPI.Printf(stderr, "Starting %d.%d\n", MPI.CommWorldSize, MPI.CommWorldRank);
        
        //______________________________________________________________________
        //
        // Get the file to process
        //______________________________________________________________________
        if( argc <= 1 )
        {
            return 0;
        }
        const string filename = argv[1];
        MPI.Printf0(stderr, "processing '%s'\n", filename.c_str());
        
        //______________________________________________________________________
        //
        // Load the matrix
        //______________________________________________________________________
        imatrix_t A;
        { ios::icstream fp(filename); if(!load_matrix(A, fp)) throw exception("no data in filename"); }
        if( A.rows != A.cols ) throw exception("matrix is not square");
        const size_t M = A.rows;
        MPI.Printf0(stderr, "M  = %3u\n", unsigned(M));
        
        //______________________________________________________________________
        //
        // count the possible occurences and their count
        //______________________________________________________________________
        const  size_t MM1 = M*(M-1);
        imap_t dict(MM1,as_capacity);
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
        
        const size_t nu = dict.size();
        MPI.Printf0(stderr, "nu = %3u\n", unsigned(nu));
        ivector_t lambda(nu,as_capacity);
        ivector_t occurs(nu,as_capacity);
        qvector_t ratio(nu,as_capacity);
        for( imap_t::iterator i = dict.begin();i!=dict.end();++i)
        {
            const count_t k = i->key;
            lambda.push_back(k);
            occurs.push_back(*i);
        }
        co_qsort(lambda, occurs);
        
        //______________________________________________________________________
        //
        // compute the ratio for the multinomial law
        //______________________________________________________________________
        {
            mpq rsum;
            for(size_t i=1;i<=nu;++i)
            {
                const mpq r(occurs[i],MM1);
                ratio.push_back(r);
                rsum += r;
            }
            if(rsum.den != 1 || rsum.num != 1)
            {
                throw exception("unexpected sum of ratio failure");
            }
        }
        
        if(MPI.IsFirst)
        {
            fflush(stderr);
            std::cerr << "lambda = " << lambda << std::endl;
            std::cerr << "occurs = " << occurs << std::endl;
            std::cerr << "ratio  = " << ratio  << std::endl;
            std::cerr.flush();
        }
        
        //______________________________________________________________________
        //
        // maximum of possible coincidences
        //______________________________________________________________________
        const size_t cmax = lambda[nu] * M;
        MPI.Printf0(stderr, "Maximum #Coincidences = %u\n", unsigned(cmax));
        
        //______________________________________________________________________
        //
        // M! and array of 1/K!
        //______________________________________________________________________
        const mpn factM = mpn::factorial(M);
        qvector_t factK(M+1,as_capacity);
        mpq K=1;
        factK.push_back(K);
        for(uint64_t i=1;i<=M;++i)
        {
            const mpq fK = factK.back()/int64_t(i);
            factK.push_back(fK);
        }
        
        if(MPI.IsFirst)
        {
            fflush(stderr);
            std::cerr << "M!   = " << factM << std::endl;
            std::cerr << "1/K! = " << factK << std::endl;
            std::cerr.flush();
        }
        
        //______________________________________________________________________
        //
        // Pre-compute all the powers
        //______________________________________________________________________
        const mpq __qone(1);
        Tableau2D<mpq> ratioPower(1,nu,0,M,__qone);
        std::cerr << "Pre-computing powers..." << std::endl;
        for(size_t i=1;i<=nu;++i)
        {
            const mpq           &q = ratio[i];
            Tableau2D<mpq>::Row &R = ratioPower[i];
            for(size_t p=1;p<=M;++p)
            {
                R[p] = mpq::mul(R[p-1],q);
            }
        }
        
        //______________________________________________________________________
        //
        // Array of probabilities to fill
        //______________________________________________________________________
        const mpq __qzero(0);
        const size_t psize = cmax+1;
        qvector_t    pdf(psize,__qzero);
        
        //______________________________________________________________________
        //
        // socks/drawers dispatcher
        //______________________________________________________________________
        dispatch  D(M,nu);
        const mpn num_combi = mpn::binomial(D.meta, D.walls);
        if( MPI.IsFirst )
        {
            fflush(stderr);
            std::cerr << "Need to Evaluate " << num_combi << " combination" << std::endl;
            std::cerr.flush();
        }
        
        //______________________________________________________________________
        //
        // dividing the task
        //______________________________________________________________________
        mpn offset; // 0 offset
        mpn length = num_combi;
        core::mpi_split(MPI.CommWorldRank, MPI.CommWorldSize, offset, length);
        const size_t ioff = offset.to<size_t>();
        const size_t nops = length.to<size_t>();
        MPI.Printf(stderr, "offset=%8u | length=%8u (%u->%u)\n", unsigned(ioff), unsigned(nops), unsigned(ioff), unsigned(ioff+nops-1) );
        
        //______________________________________________________________________
        //
        // skipping the combinations
        //______________________________________________________________________
        for(mpn i;i<offset;++i)
        {
            if(!D.next())
                throw exception("unexpected too many skipped combinations!");
        }
        
        //______________________________________________________________________
        //
        // Evaluating the combinations
        //______________________________________________________________________
        eta ETA;
        mpz done;
        ETA.reset();
        double t_last = ETA.now()-1000;
        for(mpn ic;ic<length;++ic)
        {
            //__________________________________________________________________
            //
            // do something
            //__________________________________________________________________
            ++done;
            
            //__________________________________________________________________
            //
            // the dispatcher contains the multiplicity of each occurence
            //__________________________________________________________________
            size_t ncoinc = 0;
            
            //__________________________________________________________________
            //
            // multinomial law
            //__________________________________________________________________
            mpq    proba  = factM;
            for(size_t i=nu;i>0;--i)
            {
                const size_t K = D[i-1];
                ncoinc += lambda[i] * K;
                proba  *= ratioPower[i][K];
                proba  *= factK[K+1];
            }
            pdf[ncoinc+1] += proba;
            
            //__________________________________________________________________
            //
            // next one
            //__________________________________________________________________
            if(!D.next())
            {
                break;
            }
            
            if(MPI.IsFirst)
            {
                const double t_curr = ETA.now();
                if( t_curr >= t_last + 1)
                {
                    const mpq rr(done,length);
                    ETA(rr.to_double());
                    eta_callback(MPI, ETA);
                    t_last = t_curr;
                }
            }
        }
        ETA(1);
        eta_callback(MPI,ETA);
        MPI.Printf0(stderr, "\n");
        MPI.Printf0(stderr, "Synchronizing...\n");
        if( done.n != length )
            throw exception("invalid combination count...");

        //______________________________________________________________________
        //
        // reduction of rational numbers in root
        //______________________________________________________________________
        if( MPI.IsParallel )
        {
            if(MPI.IsFirst)
            {
                for(int r=1;r<MPI.CommWorldSize;++r)
                {
                    for(size_t i=1;i<=psize;++i)
                    {
                        const mpq rq = mpa::mpi_io::recv_q(MPI,r,MPI_COMM_WORLD);
                        pdf[i] += rq;
                    }
                }
            }
            else
            {
                for(size_t i=1;i<=psize;++i)
                {
                    mpa::mpi_io::send(MPI,pdf[i],0,MPI_COMM_WORLD);
                }
            }
        }
        
        //______________________________________________________________________
        //
        // in root: cdf, real cdf and saving...
        //______________________________________________________________________
        if(MPI.IsFirst)
        {
            std::cerr << "pdf=" << pdf << std::endl;
            qvector_t qcdf(psize,__qzero);
            qcdf[1] = pdf[1];
            for(size_t i=2;i<=pdf.size();++i)
            {
                qcdf[i] = qcdf[i-1] + pdf[i];
            }
            std::cerr << "qcdf=" << qcdf << std::endl;
            const mpq &sum = qcdf.back();
            if( !(sum.num==1&&sum.den==1) )
                throw exception("error in probabilities!");
            
            vector<double> cdf(psize,0);
            for(size_t i=1;i<=psize;++i)
            {
                cdf[i] = qcdf[i].to_double();
            }
            const string outname = filename + "_xts.txt";
            ios::ocstream         out(outname,false);
            for(size_t i=1;i<=cdf.size();++i)
            {
                out("%u %.10e\n", unsigned(i-1), cdf[i]);
            }

        }
        
        
        return 0;
    }
    catch(const exception &e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << e.when() << std::endl;
    }
    catch(...)
    {
        std::cerr << "unhandle exception in " << progname << std::endl;
    }
    return -1;
}
