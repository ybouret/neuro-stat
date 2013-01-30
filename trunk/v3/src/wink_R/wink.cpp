#include "../pyck/R++.hpp"
#include "../wink/neurons.hpp"
#include "../pyck/wtime.hpp"

using namespace wink;

////////////////////////////////////////////////////////////////////////////////
//
// function to test dynamic loading
//
////////////////////////////////////////////////////////////////////////////////
extern "C"
SEXP wink_version()
{
    Rprintf("Compiled on %s\n", __DATE__);
    return R_NilValue;
}


static
neurons & shared_neurons() throw()
{
    static neurons XP;
    return XP;
}


static
Mutex & shared_mutex() throw()
{
    static Mutex M;
    return M;
}

extern "C"
SEXP wink_perm( SEXP Rn )
{
    try
    {
        static DefaultUniformGenerator &ran = shared_neurons().ran;
        
        const size_t n = R2Scalar<int>(Rn);
        if( n <= 0 )
        {
            throw Exception("required length is negative!");
        }
        
        RVector<int>         ans(n);
        for(size_t i=0; i<n;++i)
        {
            ans[i] = int(i);
        }
        ran.shuffle( &ans[0], n);
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

namespace
{
    static inline void __show_neuron( int id, const RMatrix<double> &r )
    {
        Rprintf("\tWINK: Neuron %d | #trials=%4u | max_tops=%5u\n", id, unsigned(r.rows), unsigned(r.cols-1) );
    }
    
    //! make a neuron from a R matrix
    class RNeuron :  public neuron
    {
    public:
        explicit RNeuron( const RMatrix<double> &self ) :
        neuron( self.rows, col2dat(self.cols) )
        {
            loadR( &self[0][0], self.rows, self.cols);
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
    
    //! make intervals from R intervals argument
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
    
    //! Validate the statistic name
    static inline
    statistic_value __check_stat_val( SEXP Rvalue )
    {
        const char *value = CHAR(STRING_ELT(Rvalue,0));
        if(!value) throw Exception("NULL statistic_value");
        
        if(strcmp(value,"T")==0)
        {
            return statistic_T;
        }
        
        if(strcmp(value,"H")==0)
        {
            return statistic_H;
        }
        
        throw Exception("Unknown option '%s'", value);
    }
    
    
}

////////////////////////////////////////////////////////////////////////////////
//
// true coincidences, always serial
//
////////////////////////////////////////////////////////////////////////////////
extern "C"
SEXP wink_true_coincidences( SEXP Rvalue, SEXP RN1, SEXP RN2, SEXP RI, SEXP Rdelta)
{
    try
    {
        //----------------------------------------------------------------------
        //-- parse arguments
        //----------------------------------------------------------------------
        const RMatrix<double> M1(RN1); __show_neuron(1,M1);
        RNeuron               N1(M1);
        const RMatrix<double> M2(RN2); __show_neuron(2,M2);
        RNeuron               N2(M2);
        RIntervals            intervals(RI);
        const double          delta = R2Scalar<double>(Rdelta);
        const statistic_value S     = __check_stat_val(Rvalue);
        const size_t          num   = intervals.cols;
        
        //----------------------------------------------------------------------
        //-- prepare answer
        //----------------------------------------------------------------------
        RVector<double> ans(num);
        neurons         &xp = shared_neurons();
        for(size_t i=0; i < num;++i)
        {
            const double a = intervals[i][0];
            const double b = intervals[i][1];
            ans[i] = double(xp.true_coincidences( S,  N1, N2, a, b, delta));
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
//
//
//
////////////////////////////////////////////////////////////////////////////////

#include "../pyck/team.hpp"

namespace
{
    struct WorkerArgs
    {
        RMatrix<double>       *M1;
        RMatrix<double>       *M2;
        statistic_value        S;
        const RMatrix<double> *intervals;
        double                 delta;
        size_t                 B;
        mix_method             kind;
        RMatrix<double>       *output;
        size_t                 num_threads;
        size_t                 thread_rank;
    };
    
    class Worker : public Runnable
    {
    public:
        RNeuron                N1;
        RNeuron                N2;
        const statistic_value  S;
        const RMatrix<double> &intervals;
        const double           delta;
        C_Array<count_t>       coinc;
        const mix_method       kind;
        RMatrix<double>       &output;
        size_t                 ini;
        size_t                 num;
        neurons                xp;
        
        explicit Worker(Mutex            &m,
                        const WorkerArgs &args ) :
        Runnable(m),
        N1( *args.M1 ),
        N2( *args.M2 ),
        S(   args.S ),
        intervals( *args.intervals),
        delta( args.delta ),
        coinc( args.B ),
        kind( args.kind ),
        output( *args.output ),
        ini(0),
        num(0),
        xp()
        {
            TeamBalance( intervals.cols, ini, num, args.thread_rank, args.num_threads);
        }
        
        virtual ~Worker() throw()
        {
        }
        
    private:
        Worker( const Worker & );
        Worker&operator=(const Worker &);
    };
    
    class WorkerPerm : public Worker
    {
    public:
        explicit WorkerPerm( Mutex            &m,
                            const WorkerArgs &args ) :
        Worker(m,args)
        {
        }
        
        virtual ~WorkerPerm() throw()
        {
            
        }
        
        virtual
        void run() throw()
        {
            RMatrix<double> &alpha = output;
            try
            {
                for( size_t i=ini,j=0;j<num;++i,++j)
                {
                    const double a = intervals[i][0];
                    const double b = intervals[i][1];
                    
                    //-- initialize with true coincidences
                    const size_t TrueS = double(xp.true_coincidences(S, N1, N2, a, b, delta));
                    
                    //-- permutations
                    xp.eval_coincidences(S,coinc,kind);
                    
                    //-- evaluate pvalues
                    xp.compute_pvalues(alpha[i][0],alpha[i][1],coinc,TrueS);
                }
                
            }
            catch( const std::exception &e )
            {
                Rprintf("***wink_permutation/thread: %\n", e.what());
            }
            catch (...)
            {
                Rprintf("*** unhandled error in wink_permutation/thread\n");
            }
        }
        
        
    private:
        WorkerPerm(const WorkerPerm &);
        WorkerPerm&operator=(const WorkerPerm &);
    };
    
    
}

////////////////////////////////////////////////////////////////////////////////
//
// Parallel Permutations
//
////////////////////////////////////////////////////////////////////////////////
extern "C"
SEXP wink_permutations(SEXP Ropt, SEXP RN1, SEXP RN2, SEXP RI, SEXP Rdelta, SEXP RB, SEXP RNumThreads)
{
    try
    {
        //----------------------------------------------------------------------
        //
        // Parsing Arguments Once
        //
        //----------------------------------------------------------------------
        const size_t           num_threads   = R2Scalar<int>(RNumThreads);
        if( num_threads <= 0 )
            throw Exception("Invalid #num_threads");
        Rprintf("\tWINK: Permutation[%u thread%c]\n", unsigned(num_threads), num_threads>1 ? 's' : ' ' );
        
        const mix_method       kind  = mix_perm;
        const statistic_value  S     = __check_stat_val(Ropt);
        RMatrix<double>        M1(RN1); __show_neuron(1,M1);
        RMatrix<double>        M2(RN2); __show_neuron(2,M2);
        RIntervals             intervals(RI);
        const double           delta = R2Scalar<double>(Rdelta);
        const size_t           B     = R2Scalar<int>(RB);
        
        
        //----------------------------------------------------------------------
        //-- prepare answer
        //-- first  row: alpha_minus
        //-- second row: alpha_plus
        //----------------------------------------------------------------------
        const size_t    num  = intervals.cols;
        RMatrix<double> alpha(2,num);
        
        Rprintf("\tWINK: #intervals  = %u\n", unsigned(num));
        Rprintf("\tWINK: #bootstraps = %u\n", unsigned(B));
        
        //----------------------------------------------------------------------
        //
        // Preparing the arguments
        //
        //----------------------------------------------------------------------
        WorkerArgs       args = { &M1, &M2, S, &intervals, delta, B, kind, &alpha, num_threads, 0 };
        
        if( num_threads > 1 )
        {
            //-- parallel
            Rprintf("\tWINK: <PARALLEL CODE>\n");
            Team<WorkerPerm> team(num_threads);
            for(size_t i=0;i<num_threads;++i)
            {
                args.thread_rank = i;
                team.enqueue( args );
            }
            team.finish(); //-- not necessary, called by destructor before returning
        }
        else
        {
            //-- serial
            Rprintf("\tWINK: <SERIAL CODE>\n");
            WorkerPerm work( shared_mutex(), args );
            work.run();
        }
        //----------------------------------------------------------------------
        //
        // All Done
        //
        //----------------------------------------------------------------------
        return *alpha;
    }
    catch( const Exception &e )
    {
        Rprintf("*** wink_permutation_par: %s\n", e.what());
    }
    catch(...)
    {
        Rprintf("*** unhanled exception in wink_permutation_par\n");
    }
    return R_NilValue;
}






