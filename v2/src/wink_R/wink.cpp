#include "../pyck/R++.hpp"
#include "../pyck/exception.hpp"
#include "../pyck/wtime.hpp"
#include "../wink/neurons.hpp"

using namespace wink;

////////////////////////////////////////////////////////////////////////////////
//
// Used to detect if the DLL is loaded
//
////////////////////////////////////////////////////////////////////////////////
extern "C" SEXP wink_version()
{
    Rprintf("Compiled on %s\n", __DATE__);
    return R_NilValue;
}


////////////////////////////////////////////////////////////////////////////////
//
// Wrapper classes C++ <=> R
//
////////////////////////////////////////////////////////////////////////////////
namespace {
    
    class RNeuronWrapper
    {
    public:
        RMatrix<double> self;
        
        explicit RNeuronWrapper( SEXP Rmat ) :
        self(Rmat)
        {
        }
        
    private:
        RNeuronWrapper(const RNeuronWrapper&);
        RNeuronWrapper&operator=(const RNeuronWrapper&);
        
    };
    
    class RNeuron : public RNeuronWrapper, public neuron
    {
    public:
        explicit RNeuron( SEXP Rmat ) :
        RNeuronWrapper(Rmat),
        neuron( self.rows, col2dat(self.cols) )
        {
            loadR( &self[0][0], self.rows, self.cols);
            Rprintf("\tWINK: Neuron: #trials=%u, #max_tops=%u\n", trials, length );
        }
        
        virtual ~RNeuron() throw()
        {
        }
        
        
    private:
        static inline
        size_t col2dat(size_t nc )
        {
            if( nc < 1 )
                throw Exception("RNeuron: invalid #cols");
            return nc-1;
        }
        
        RNeuron(const RNeuron &);
        RNeuron&operator=(const RNeuron &);
        
    };
    
    class RIntervals : public RMatrix<double>
    {
    public:
        explicit RIntervals( SEXP RI ) :
        RMatrix<double>(RI)
        {
            if( rows != 2 )
                throw Exception("RIntervals: invalid #rows=%u", unsigned(rows));
        }
        
        virtual ~RIntervals() throw()
        {
        }
        
    private:
        RIntervals(const RIntervals &);
        RIntervals&operator=(const RIntervals &);
        
    };
    
    template <typename T>
    static inline
    T R2( SEXP Rvec )
    {
        RVector<T> value(Rvec);
        if( value.size < 1) throw Exception("R2<Type>: bad vector size");
        return value[0];
    }
    
    static DefaultUniformGenerator shared_ran;
    static Mutex                   shared_mutex;
}

////////////////////////////////////////////////////////////////////////////////
//
// Generate a permutation
//
////////////////////////////////////////////////////////////////////////////////
extern "C" SEXP wink_perm( SEXP Rn )
{
    try
    {
        const size_t n = R2<int>(Rn);
        if( n <= 0 )
        {
            throw Exception("required length is negative!");
        }
        
        RVector<int>         ans(n);
        for(size_t i=0; i<n;++i)
        {
            ans[i] = int(i);
        }
        
        PYCK_LOCK(shared_mutex);
        shared_ran.shuffle( &ans[0], n);
        
        return *ans;
    }
    catch( const Exception &e )
    {
        Rprintf("*** wink_perm: %s\n", e.what());
    }
    catch(...)
    {
        Rprintf("*** unhanled exception in wink_perm\n");
    }
    return R_NilValue;
}

////////////////////////////////////////////////////////////////////////////////
//
// Compute the number of true coincidences on intervals
//
////////////////////////////////////////////////////////////////////////////////
extern "C"
SEXP wink_true_coincidences( SEXP RN1, SEXP RN2, SEXP RI, SEXP Rdelta)
{
    
    try
    {
        //----------------------------------------------------------------------
        //-- parse arguments
        //----------------------------------------------------------------------
        RNeuron         N1(RN1);
        RNeuron         N2(RN2);
        RIntervals      intervals(RI);
        const double    delta = R2<double>(Rdelta);
        
        const size_t num_intervals = intervals.cols;
        
        //----------------------------------------------------------------------
        //-- prepare answer
        //----------------------------------------------------------------------
        RVector<double> ans(num_intervals);
        neurons         xp;
        for(size_t i=0; i < num_intervals;++i)
        {
            const double a = intervals[i][0];
            const double b = intervals[i][1];
            ans[i] = double(xp.true_coincidences(N1, N2, a, b, delta));
        }
        //----------------------------------------------------------------------
        //-- done
        //----------------------------------------------------------------------
        return *ans;
    }
    catch( const Exception &e )
    {
        Rprintf("*** wink_true_coincidences: %s\n", e.what());
    }
    catch(...)
    {
        Rprintf("*** unhanled exception in wink_true_coincidences\n");
    }
    return R_NilValue;
}

////////////////////////////////////////////////////////////////////////////////
//
// Serial Bootstrap
//
////////////////////////////////////////////////////////////////////////////////
static inline
bootstrap_method __check_option( SEXP Ropt )
{
    const char *option = CHAR(STRING_ELT(Ropt,0));
    if(!option) throw Exception("NULL option");
    
    if(strcmp(option,"perm")==0)
    {
        return bootstrap_perm;
    }
    
    if(strcmp(option,"repl")==0)
    {
        return bootstrap_repl;
    }
    
    throw Exception("Unknown option '%s'", option);
    
}

extern "C"
SEXP wink_bootstrap(SEXP RN1, SEXP RN2, SEXP RI, SEXP Rdelta, SEXP RB, SEXP Ropt)
{
    try
    {
        //----------------------------------------------------------------------
        //-- check option
        //----------------------------------------------------------------------
        const bootstrap_method Bkind = __check_option(Ropt);
        
        //----------------------------------------------------------------------
        //-- parse arguments
        //----------------------------------------------------------------------
        RNeuron         N1(RN1);
        RNeuron         N2(RN2);
        RIntervals      intervals(RI);
        const double    delta         = R2<double>(Rdelta);
        const size_t    nb            = R2<int>(RB);
        const size_t    num_intervals = intervals.cols;
        
        Rprintf("\tWINK: #intervals  = %u\n", unsigned(num_intervals));
        Rprintf("\tWINK: #bootstraps = %u\n", unsigned(nb));
        
        //----------------------------------------------------------------------
        //-- prepare answer
        //-- first  row: alpha_minus
        //-- second row: alpha_plus
        //----------------------------------------------------------------------
        RMatrix<double> alpha(2,num_intervals);
        neurons         xp;
        C_Array<size_t> Bcoinc( nb );
        for(size_t i=0; i < num_intervals;++i)
        {
            const double a = intervals[i][0];
            const double b = intervals[i][1];
            
            //-- initialize with true coincidences
            const size_t Tcoinc = double(xp.true_coincidences(N1, N2, a, b, delta));
            
            //-- bootstrap
            xp.bootstrap(Bcoinc, Bkind, N1, N2, delta);
            
            //-- evaluate pvalues
            xp.compute_pvalues_T(alpha[i][0],alpha[i][1],Bcoinc,Tcoinc);
        }
        
        //----------------------------------------------------------------------
        //-- done
        //----------------------------------------------------------------------
        return *alpha;
    }
    catch( const Exception &e )
    {
        Rprintf("*** wink_bootstrap: %s\n", e.what());
    }
    catch(...)
    {
        Rprintf("*** unhanled exception in wink_bootstrap\n");
    }
    return R_NilValue;
}

#include "../pyck/team.hpp"

namespace
{
    struct WorkerArgs
    {
        neuron                 *N1;
        neuron                 *N2;
        const RMatrix<double> *intervals;
        double                 delta;
        size_t                 B;
        bootstrap_method       Bkind;
        RMatrix<double>       *alpha;
        size_t                 num_threads;
        size_t                 thread_rank;
    };
    
    class Worker : public Runnable
    {
    public:
        neuron                &N1;
        neuron                &N2;
        const RMatrix<double> &intervals;
        const double           delta;
        const size_t           B;
        C_Array<size_t>        Bcoinc;
        const bootstrap_method Bkind;
        RMatrix<double>       &alpha;
        size_t                 ini;
        size_t                 num;
        neurons                xp;
        
        explicit Worker( Mutex &m,
                        const WorkerArgs &args ) :
        Runnable(m),
        N1( *args.N1 ),
        N2( *args.N2 ),
        intervals( *args.intervals),
        delta( args.delta ),
        B( args.B ),
        Bcoinc(B),
        Bkind( args.Bkind ),
        alpha( *args.alpha ),
        ini(0),
        num(0),
        xp()
        {
            TeamBalance( intervals.cols, ini, num, args.thread_rank, args.num_threads);
        }
        
        virtual ~Worker() throw()
        {
        }
        
        virtual
        void run() throw()
        {
            {
                PYCK_LOCK(mutex);
                std::cerr << "...Starting Worker: " << ini << " +" << num << std::endl;
            }
            try
            {
                for( size_t i=ini,j=0;j<num;++i,++j)
                {
                    const double a = intervals[i][0];
                    const double b = intervals[i][1];
                    
                    //-- initialize with true coincidences
                    const size_t Tcoinc = double(xp.true_coincidences(N1, N2, a, b, delta));
                    
                    //-- bootstrap
                    xp.bootstrap(Bcoinc, Bkind, N1, N2, delta);
                    
                    //-- evaluate pvalues
                    xp.compute_pvalues_T(alpha[i][0],alpha[i][1],Bcoinc,Tcoinc);
                }
            }
            catch( const std::exception &e )
            {
                Rprintf("***wink_bootstrap_par/thread: %\n", e.what());
            }
            catch (...)
            {
                Rprintf("*** unhandled error in wink_bootstrap_par/thread\n");
            }
        }
        
    private:
        Worker( const Worker & );
        Worker&operator=(const Worker &);
    };
    
}

////////////////////////////////////////////////////////////////////////////////
//
// Parallel Bootstrap
//
////////////////////////////////////////////////////////////////////////////////
extern "C"
SEXP wink_bootstrap_par(SEXP RN1, SEXP RN2, SEXP RI, SEXP Rdelta, SEXP RB, SEXP Ropt, SEXP RNumThreads)
{
    try
    {
        //----------------------------------------------------------------------
        //
        // Parsing Arguments Once
        //
        //----------------------------------------------------------------------
        const bootstrap_method Bkind = __check_option(Ropt);
        RNeuron                N1(RN1);
        RNeuron                N2(RN2);
        RIntervals             intervals(RI);
        const double           delta         = R2<double>(Rdelta);
        const size_t           B             = R2<int>(RB);
        const size_t           num_threads   = R2<int>(RNumThreads);
        if( num_threads <= 0 )
            throw Exception("Invalide #num_threads");
        
        //----------------------------------------------------------------------
        //-- prepare answer
        //-- first  row: alpha_minus
        //-- second row: alpha_plus
        //----------------------------------------------------------------------
        const size_t num_intervals = intervals.cols;
        RMatrix<double> alpha(2,num_intervals);
        neurons         xp;
        
        //----------------------------------------------------------------------
        //
        // Launching the team
        //
        //----------------------------------------------------------------------
        Team<Worker> team(num_threads);
        WorkerArgs   args = { &N1, &N2, &intervals, delta, B, Bkind, &alpha, num_threads };
        for(size_t i=0;i<num_threads;++i)
        {
            args.thread_rank = i;
            team.enqueue( args );
        }
        
        return *alpha;
    }
    catch( const Exception &e )
    {
        Rprintf("*** wink_bootstrap_par: %s\n", e.what());
    }
    catch(...)
    {
        Rprintf("*** unhanled exception in wink_bootstrap_par\n");
    }
    return R_NilValue;
}




