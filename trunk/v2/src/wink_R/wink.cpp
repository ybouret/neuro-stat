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
    
    
    static inline void __show_neuron( const RMatrix<double> &r )
    {
        Rprintf("\tWINK: Neuron with #trials=%u\n", unsigned(r.rows));
    }
    
    class RNeuron :  public neuron
    {
    public:
        explicit RNeuron( const RMatrix<double> &self ) :
        neuron( self.rows, col2dat(self.cols) )
        {
            loadR( &self[0][0], self.rows, self.cols);
            //Rprintf("\tWINK: Neuron: #trials=%u, #max_tops=%u\n", trials, length );
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
    
    static Mutex                   shared_mutex;
    class SharedRan : public DefaultUniformGenerator
    {
    public:
        explicit SharedRan()
        {
            seed( WallTime:: Seed() );
        }
        
        
        virtual ~SharedRan() throw()
        {
            
        }
        
    private:
        SharedRan(const SharedRan&);
        SharedRan&operator=(const SharedRan&);

    };
    
    static SharedRan shared_ran;

   
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

extern "C"
SEXP wink_true_coincidences( SEXP RN1, SEXP RN2, SEXP RI, SEXP Rdelta, SEXP Rvalue)
{
    try
    {
        //----------------------------------------------------------------------
        //-- parse arguments
        //----------------------------------------------------------------------
        const RMatrix<double> M1(RN1); __show_neuron(M1);
        RNeuron               N1(M1);
        const RMatrix<double> M2(RN2); __show_neuron(M2);
        RNeuron               N2(M2);
        RIntervals            intervals(RI);
        const double          delta = R2<double>(Rdelta);
        const statistic_value S     = __check_stat_val(Rvalue);
        const size_t          num_intervals = intervals.cols;
        
        //----------------------------------------------------------------------
        //-- prepare answer
        //----------------------------------------------------------------------
        RVector<double> ans(num_intervals);
        neurons         xp;
        for(size_t i=0; i < num_intervals;++i)
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
// Serial Permutations
//
////////////////////////////////////////////////////////////////////////////////
extern "C"
SEXP wink_permutation(SEXP RN1, SEXP RN2, SEXP RI, SEXP Rdelta, SEXP RB, SEXP Ropt)
{
    try
    {
        //----------------------------------------------------------------------
        //-- check option
        //----------------------------------------------------------------------
        const mix_method       Bkind = mix_perm; //__check_option(Ropt);
        const statistic_value  S     = __check_stat_val(Ropt);
        Rprintf("\tWINK: Serial Permutation Code\n");

        //----------------------------------------------------------------------
        //-- parse arguments
        //----------------------------------------------------------------------
        const RMatrix<double> M1(RN1); __show_neuron(M1);
        RNeuron               N1(M1);
        const RMatrix<double> M2(RN2); __show_neuron(M2);
        RNeuron               N2(M2);
        RIntervals            intervals(RI);
        const double          delta         = R2<double>(Rdelta);
        const size_t          nb            = R2<int>(RB);
        const size_t          num_intervals = intervals.cols;
        
        Rprintf("\tWINK: #intervals  = %u\n", unsigned(num_intervals));
        Rprintf("\tWINK: #bootstraps = %u\n", unsigned(nb));
        
        //----------------------------------------------------------------------
        //-- prepare answer
        //-- first  row: alpha_minus
        //-- second row: alpha_plus
        //----------------------------------------------------------------------
        RMatrix<double>  alpha(2,num_intervals);
        neurons          xp;
        C_Array<count_t> Bcoinc( nb );
        for(size_t i=0; i < num_intervals;++i)
        {
            const double a = intervals[i][0];
            const double b = intervals[i][1];
            
            //-- initialize with true coincidences
            const size_t Tcoinc = double(xp.true_coincidences( S, N1, N2, a, b, delta));
            
            //-- mix'em all
            xp.mix( S, Bcoinc, Bkind, N1, N2, delta);
            
            //-- evaluate pvalues
            xp.compute_pvalues(alpha[i][0],alpha[i][1],Bcoinc,Tcoinc);
        }
        
        //----------------------------------------------------------------------
        //-- done
        //----------------------------------------------------------------------
        return *alpha;
    }
    catch( const Exception &e )
    {
        Rprintf("*** wink_permutation: %s\n", e.what());
    }
    catch(...)
    {
        Rprintf("*** unhanled exception in wink_permutation\n");
    }
    return R_NilValue;
}

extern "C"
SEXP wink_bootstrap(SEXP RN1, SEXP RN2, SEXP RI, SEXP Rdelta, SEXP RB)
{
    try
    {
        Rprintf("\tWINK: Serial Bootstrap Code\n");
        
        //----------------------------------------------------------------------
        //-- parse arguments
        //----------------------------------------------------------------------
        const RMatrix<double> M1(RN1); __show_neuron(M1);
        RNeuron               N1(M1);
        const RMatrix<double> M2(RN2); __show_neuron(M2);
        RNeuron               N2(M2);
        RIntervals            intervals(RI);
        const double          delta         = R2<double>(Rdelta);
        const size_t          nb            = R2<int>(RB);
        const size_t          num_intervals = intervals.cols;
        
        Rprintf("\tWINK: #intervals  = %u\n", unsigned(num_intervals));
        Rprintf("\tWINK: #bootstraps = %u\n", unsigned(nb));
        
        
        //----------------------------------------------------------------------
        //-- prepare answer
        //-- first  row: alpha_minus
        //-- second row: alpha_plus
        //----------------------------------------------------------------------
        RMatrix<double>  counts(2,num_intervals);
        neurons          xp;
        C_Array<count_t> Bcoinc( nb );
        for(size_t i=0; i < num_intervals;++i)
        {
            const double a = intervals[i][0];
            const double b = intervals[i][1];
            
            //-- initialize with true coincidences
            const size_t H  = double(xp.true_coincidences( statistic_H, N1, N2, a, b, delta));
            
            //-- mix'em all
            xp.mix( statistic_H, Bcoinc, mix_perm, N1, N2, delta);
            
            //-- evaluate pvalues
            xp.compute_counts(counts[i][0],counts[i][1],Bcoinc,H);
        }
        
        //----------------------------------------------------------------------
        //-- done
        //----------------------------------------------------------------------
        return *counts;

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


////////////////////////////////////////////////////////////////////////////////
//
//
// Parallel Code
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
        mix_method             Bkind;
        RMatrix<double>       *alpha;
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
        const size_t           B;
        C_Array<count_t>       Bcoinc;
        const mix_method       Bkind;
        RMatrix<double>       &alpha;
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
            try
            {
                for( size_t i=ini,j=0;j<num;++i,++j)
                {
                    const double a = intervals[i][0];
                    const double b = intervals[i][1];
                    
                    //-- initialize with true coincidences
                    const size_t Tcoinc = double(xp.true_coincidences(S, N1, N2, a, b, delta));
                    
                    //-- bootstrap
                    xp.mix( S, Bcoinc, Bkind, N1, N2, delta);
                    
                    //-- evaluate pvalues
                    xp.compute_pvalues(alpha[i][0],alpha[i][1],Bcoinc,Tcoinc);
                }
            }
            catch( const std::exception &e )
            {
                Rprintf("***wink_permutation_par/thread: %\n", e.what());
            }
            catch (...)
            {
                Rprintf("*** unhandled error in wink_permutation_par/thread\n");
            }
        }
        
    private:
        Worker( const Worker & );
        Worker&operator=(const Worker &);
    };
    
}

////////////////////////////////////////////////////////////////////////////////
//
// Parallel Permutations
//
////////////////////////////////////////////////////////////////////////////////
extern "C"
SEXP wink_permutation_par(SEXP RN1, SEXP RN2, SEXP RI, SEXP Rdelta, SEXP RB, SEXP Ropt, SEXP RNumThreads)
{
    try
    {
        //----------------------------------------------------------------------
        //
        // Parsing Arguments Once
        //
        //----------------------------------------------------------------------
        const size_t           num_threads   = R2<int>(RNumThreads);
        if( num_threads <= 0 )
            throw Exception("Invalid #num_threads");
        Rprintf("\tWINK: Parallel Permutation Code [%u thread%c]\n", unsigned(num_threads), num_threads>1 ? 's' : ' ' );
        
        const mix_method       Bkind = mix_perm;
        const statistic_value  S     = __check_stat_val(Ropt);
        RMatrix<double>        M1(RN1); __show_neuron(M1);
        RMatrix<double>        M2(RN2); __show_neuron(M2);
        RIntervals             intervals(RI);
        const double           delta         = R2<double>(Rdelta);
        const size_t           B             = R2<int>(RB);
        
        //----------------------------------------------------------------------
        //-- prepare answer
        //-- first  row: alpha_minus
        //-- second row: alpha_plus
        //----------------------------------------------------------------------
        const size_t num_intervals = intervals.cols;
        RMatrix<double> alpha(2,num_intervals);
        
        Rprintf("\tWINK: #intervals  = %u\n", unsigned(num_intervals));
        Rprintf("\tWINK: #bootstraps = %u\n", unsigned(B));
        
        //----------------------------------------------------------------------
        //
        // Launching the team
        //
        //----------------------------------------------------------------------
        Team<Worker> team(num_threads);
        WorkerArgs   args = { &M1, &M2, S, &intervals, delta, B, Bkind, &alpha, num_threads };
        for(size_t i=0;i<num_threads;++i)
        {
            args.thread_rank = i;
            team.enqueue( args );
        }
        
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




