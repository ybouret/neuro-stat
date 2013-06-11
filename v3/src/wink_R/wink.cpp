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

////////////////////////////////////////////////////////////////////////////////
//
// function to test the permutation
//
////////////////////////////////////////////////////////////////////////////////
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
        
        RVector<int> ans(n);
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


////////////////////////////////////////////////////////////////////////////////
//
// Interface R/C++
//
////////////////////////////////////////////////////////////////////////////////

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
        const char *value = R2String(Rvalue);
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
SEXP wink_true_coincidences(SEXP Rvalue,
                            SEXP RN1,
                            SEXP RN2,
                            SEXP RI,
                            SEXP Rdelta)
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
// Base code for Parallel Work
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
        Worker(m,args) {}
        
        virtual ~WorkerPerm() throw() {}
        
        virtual void run() throw()
        {
            RMatrix<double> &alpha = output;
            try
            {
                //==============================================================
                //
                // Loops on my share or work...
                //
                //==============================================================
                for( size_t i=ini,j=0;j<num;++i,++j)
                {
                    const double a = intervals[i][0];
                    const double b = intervals[i][1];
                    
                    //----------------------------------------------------------
                    //-- initialize with true coincidences
                    //----------------------------------------------------------
                    const count_t TrueS = xp.true_coincidences(S, N1, N2, a, b, delta);
                    
                    //----------------------------------------------------------
                    //-- permutations
                    //----------------------------------------------------------
                    xp.eval_coincidences(S,coinc,kind);
                    
                    //----------------------------------------------------------
                    //-- evaluate pvalues
                    //----------------------------------------------------------

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
    
    
    class WorkerBoot : public Worker
    {
    public:
        explicit WorkerBoot(Mutex            &m,
                            const WorkerArgs &args ) :
        Worker(m,args)
        {
        }
        
        virtual ~WorkerBoot() throw()
        {
            
        }
        
        virtual
        void run() throw()
        {
            RMatrix<double> &counts = output;
            const size_t     nb     = coinc.size;
            try
            {
                //==============================================================
                //
                // Loops on my share or work...
                //
                //==============================================================
                for( size_t i=ini,j=0;j<num;++i,++j)
                {
                    const double a = intervals[i][0];
                    const double b = intervals[i][1];
                    
                    //----------------------------------------------------------
                    //-- initialize with true coincidences
                    //----------------------------------------------------------
                    const size_t H  = double(xp.true_coincidences( statistic_H, N1, N2, a, b, delta));
                    
                    //----------------------------------------------------------
                    //-- mix'em all, bootstrap kind
                    //----------------------------------------------------------
                    xp.eval_coincidences( statistic_H, coinc, kind);
                    
#if 0
                    //-- center
                    for(size_t k=0; k < nb; ++k )
                        coinc[k] -= H;
#endif
                    
                    //----------------------------------------------------------
                    //-- evaluate counts
                    //----------------------------------------------------------
                    xp.compute_counts(counts[i][0],counts[i][1],coinc,H);
                }
            }
            catch( const std::exception &e )
            {
                Rprintf("***wink_bootstrap_counts/thread: %\n", e.what());
            }
            catch (...)
            {
                Rprintf("*** unhandled error in wink_bootstrap_counts/thread\n");
            }
        }
        
        
    private:
        WorkerBoot(const WorkerBoot &);
        WorkerBoot&operator=(const WorkerBoot &);
    };
    
    
}

////////////////////////////////////////////////////////////////////////////////
//
// Parallel/Serial Permutations
//
////////////////////////////////////////////////////////////////////////////////
extern "C"
SEXP wink_permutation_pvalues(SEXP Ropt,
                              SEXP RN1,
                              SEXP RN2,
                              SEXP RI,
                              SEXP Rdelta,
                              SEXP RB,
                              SEXP RNumThreads)
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

////////////////////////////////////////////////////////////////////////////////
//
// Parallel/Serial Bootstrap Counts
//
////////////////////////////////////////////////////////////////////////////////

static inline
mix_method __parse_mix( SEXP Rmix )
{
    const char            *mix_name      = R2String(Rmix);
    if( 0 == strcmp("",mix_name) )
    {
        return mix_boot;
    }
    
    if( 0 == strcmp("tau",mix_name) )
    {
        return mix_repl;
    }
    
    throw Exception("Invalid mix_name=%s",mix_name);
}

extern "C"
SEXP wink_bootstrap_counts(SEXP RN1,
                           SEXP RN2,
                           SEXP RI,
                           SEXP Rdelta,
                           SEXP RB,
                           SEXP RNumThreads,
                           SEXP Rmix)
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
        Rprintf("\tWINK: Bootstrap Counts [%u thread%c]\n", unsigned(num_threads), num_threads>1 ? 's' : ' ' );
        
        RMatrix<double>        M1(RN1); __show_neuron(1,M1);
        RMatrix<double>        M2(RN2); __show_neuron(2,M2);
        RIntervals             intervals(RI);
        const double           delta         = R2Scalar<double>(Rdelta);
        const size_t           B             = R2Scalar<int>(RB);        
        const mix_method       kind          = __parse_mix(Rmix);
        
        //----------------------------------------------------------------------
        //-- prepare answer
        //-- first  row: count_minus
        //-- second row: count_plus
        //----------------------------------------------------------------------
        const size_t    num  = intervals.cols;
        RMatrix<double> counts(2,num);
        
        Rprintf("\tWINK: #intervals  = %6u\n", unsigned(num));
        Rprintf("\tWINK: #bootstraps = %6u\n", unsigned(B));
        Rprintf("\tWINK: Mix method  = <%s>\n",R2String(Rmix));
        
        //----------------------------------------------------------------------
        //
        // Launching the team
        //
        //----------------------------------------------------------------------
        WorkerArgs       args = { &M1, &M2, statistic_H, &intervals, delta, B, kind, &counts, num_threads,0 };
        
        if( num_threads > 1 )
        {
            Team<WorkerBoot> team(num_threads);
            Rprintf("\tWINK: <PARALLEL CODE>\n");
            for(size_t i=0;i<num_threads;++i)
            {
                args.thread_rank = i;
                team.enqueue( args );
            }
            team.finish();
        }
        else
        {
            WorkerBoot worker( shared_mutex(), args );
            Rprintf("\tWINK: <SERIAL CODE>\n");
            worker.run();
        }
        //----------------------------------------------------------------------
        //
        // All done
        //
        //----------------------------------------------------------------------
        return *counts;
    }
    catch( const Exception &e )
    {
        Rprintf("*** wink_bootstrap_counts: %s\n", e.what());
    }
    catch(...)
    {
        Rprintf("*** unhanled exception in wink_bootstrap_counts\n");
    }
    return R_NilValue;
}

////////////////////////////////////////////////////////////////////////////////
//
// Parallel/Serial Bootstrap Counts for a SINGLE data
//
////////////////////////////////////////////////////////////////////////////////
#include "../pyck/sort.hpp"

extern "C"
SEXP wink_single_perm(SEXP Ropt, SEXP RN1, SEXP RN2, SEXP Ra, SEXP Rb, SEXP Rdelta, SEXP RB)
{
    try
    {
        
        //----------------------------------------------------------------------
        //-- parse arguments
        //----------------------------------------------------------------------
        const RMatrix<double> M1(RN1);
        RNeuron               N1(M1);
        const RMatrix<double> M2(RN2);
        RNeuron               N2(M2);
        const double          a     = R2Scalar<double>(Ra);
        const double          b     = R2Scalar<double>(Rb);
        const double          delta = R2Scalar<double>(Rdelta);
        const size_t          nb    = R2Scalar<int>(RB);
        neurons              &xp    = shared_neurons();
        const statistic_value  S    = __check_stat_val(Ropt);
        
        //----------------------------------------------------------------------
        //-- initialize window
        //----------------------------------------------------------------------
        const count_t    T = xp.true_coincidences( S, N1, N2, a, b, delta);
        C_Array<count_t> Tp(nb);
        
        //----------------------------------------------------------------------
        //-- mix'em all, permutation kind
        //----------------------------------------------------------------------
        xp.eval_coincidences(S,Tp,mix_perm);
        
        
        //----------------------------------------------------------------------
        //-- sort
        //----------------------------------------------------------------------
        Sort( &Tp[0], nb );
        
        //----------------------------------------------------------------------
        //-- make a list S/Sp
        //----------------------------------------------------------------------
        const char *names[] = { "S", "Sp" };
        
        //-- first element: Satistic Value
        RVector<double> rT(1);
        rT[0] = T;
        
        //-- second element: Permutation Counts
        RVector<double> rTp(nb);
        for( size_t j=0; j < nb; ++j )
            rTp[j] = double( Tp[j] );
        
        RList L(names,sizeof(names)/sizeof(names[0]));
        L.set(0,rT);
        L.set(1,rTp);
        return *L;
        
    }
    catch( const Exception &e )
    {
        Rprintf("*** wink_single_perm: %s\n", e.what());
    }
    catch(...)
    {
        Rprintf("*** unhanled exception in wink_single_perm\n");
    }
    return R_NilValue;
}

extern "C"
SEXP wink_single_boot(SEXP RN1,
                      SEXP RN2,
                      SEXP Ra,
                      SEXP Rb,
                      SEXP Rdelta,
                      SEXP RB,
                      SEXP Rmix)
{
    try
    {
        
        //----------------------------------------------------------------------
        //-- parse arguments
        //----------------------------------------------------------------------
        const RMatrix<double> M1(RN1);
        RNeuron               N1(M1);
        const RMatrix<double> M2(RN2);
        RNeuron               N2(M2);
        const double          a     = R2Scalar<double>(Ra);
        const double          b     = R2Scalar<double>(Rb);
        const double          delta = R2Scalar<double>(Rdelta);
        const size_t          nb    = R2Scalar<int>(RB);
        neurons              &xp    = shared_neurons();
        const mix_method      kind  = __parse_mix(Rmix);
        
        //----------------------------------------------------------------------
        //-- initialize window
        //----------------------------------------------------------------------
        const count_t    H = xp.true_coincidences( statistic_H, N1, N2, a, b, delta);
        C_Array<count_t> Hc(nb);
        
        //----------------------------------------------------------------------
        //-- mix'em all, bootstrap kind
        //----------------------------------------------------------------------
        xp.eval_coincidences( statistic_H,Hc,kind);
        
#if 0
        //----------------------------------------------------------------------
        //-- centering
        //----------------------------------------------------------------------
        for( size_t i=0;i<nb;++i)
            Hc[i] -= H;
#endif
        
        //----------------------------------------------------------------------
        //-- sort
        //----------------------------------------------------------------------
        Sort( &Hc[0], nb );
        
        //----------------------------------------------------------------------
        //-- make a list S/Sp
        //----------------------------------------------------------------------
        const char *names[] = { "H", "Hc" };
        
        //-- first element: H
        RVector<double> rH(1);
        rH[0] = H;
        
        //-- second element: H centered
        RVector<double> rHc(nb);
        for( size_t j=0; j < nb; ++j )
            rHc[j] = double( Hc[j] );
        
        RList L(names,sizeof(names)/sizeof(names[0]));
        L.set(0,rH);
        L.set(1,rHc);
        return *L;
        
    }
    catch( const Exception &e )
    {
        Rprintf("*** wink_single_boot: %s\n", e.what());
    }
    catch(...)
    {
        Rprintf("*** unhanled exception in wink_single_boot\n");
    }
    return R_NilValue;
}

