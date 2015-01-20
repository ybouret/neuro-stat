#include "../wink/neurons.hpp"
#include "../pyck/wtime.hpp"
#include "../pyck/R++.hpp"

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
            //const size_t     nb     = coinc.size;
            try
            {
                //==============================================================
                //
                // Loops on my share of work...
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


    class WorkerTS : public Worker
    {
    public:
        explicit WorkerTS(Mutex            &m,
                          const WorkerArgs &args ) :
        Worker(m,args)
        {
        }

        virtual ~WorkerTS() throw()
        {

        }

        virtual
        void run() throw()
        {
            RMatrix<double> &counts = output;
            //const size_t     nb     = coinc.size;
            try
            {
                //==============================================================
                //
                // Loops on my share of work...
                //
                //==============================================================
                for( size_t i=ini,j=0;j<num;++i,++j)
                {
                    const double a = intervals[i][0];
                    const double b = intervals[i][1];

                    //----------------------------------------------------------
                    //-- initialize with true coincidences
                    //----------------------------------------------------------
                    const size_t T  = double(xp.true_coincidences( statistic_T, N1, N2, a, b, delta));

                    //----------------------------------------------------------
                    //-- mix'em all, TS kind
                    //----------------------------------------------------------
                    xp.eval_coincidences( statistic_T, coinc, mix_shuf);


                    //----------------------------------------------------------
                    //-- evaluate counts
                    //----------------------------------------------------------
                    xp.compute_counts(counts[i][0],counts[i][1],coinc,T);
                }
            }
            catch( const std::exception &e )
            {
                Rprintf("***wink_TS_counts/thread: %\n", e.what());
            }
            catch (...)
            {
                Rprintf("*** unhandled error in wink_TS_counts/thread\n");
            }
        }


    private:
        WorkerTS(const WorkerTS &);
        WorkerTS&operator=(const WorkerTS &);
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

    if( 0 == strcmp("TS",mix_name) )
    {
        return mix_shuf;
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

extern "C"
SEXP wink_TS_counts(SEXP RN1,
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
        Rprintf("\tWINK: Bootstrap Counts [%u thread%c]\n", unsigned(num_threads), num_threads>1 ? 's' : ' ' );

        RMatrix<double>        M1(RN1); __show_neuron(1,M1);
        RMatrix<double>        M2(RN2); __show_neuron(2,M2);
        RIntervals             intervals(RI);
        const double           delta         = R2Scalar<double>(Rdelta);
        const size_t           B             = R2Scalar<int>(RB);
        const mix_method       kind          = mix_shuf;

        //----------------------------------------------------------------------
        //-- prepare answer
        //-- first  row: count_minus
        //-- second row: count_plus
        //----------------------------------------------------------------------
        const size_t    num  = intervals.cols;
        RMatrix<double> counts(2,num);

        Rprintf("\tWINK: #intervals  = %6u\n", unsigned(num));
        Rprintf("\tWINK: #bootstraps = %6u\n", unsigned(B));
        //Rprintf("\tWINK: Mix method  = <%s>\n",R2String(Rmix));

        //----------------------------------------------------------------------
        //
        // Launching the team
        //
        //----------------------------------------------------------------------
        WorkerArgs       args = { &M1, &M2, statistic_T, &intervals, delta, B, kind, &counts, num_threads,0 };

        if( num_threads > 1 )
        {
            Team<WorkerTS> team(num_threads);
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
            WorkerTS worker( shared_mutex(), args );
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
        Rprintf("*** wink_TS_counts: %s\n", e.what());
    }
    catch(...)
    {
        Rprintf("*** unhanled exception in wink_TS_counts\n");
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


        //----------------------------------------------------------------------
        //-- sort
        //----------------------------------------------------------------------
        Sort( &Hc[0], nb );

        //----------------------------------------------------------------------
        //-- make a list H/Hc
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


extern "C"
SEXP wink_single_TS(SEXP Ropt, SEXP RN1, SEXP RN2, SEXP Ra, SEXP Rb, SEXP Rdelta, SEXP RB)
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
        //-- mix'em all, trial shuffling style
        //----------------------------------------------------------------------
        xp.eval_coincidences(S,Tp,mix_shuf);


        //----------------------------------------------------------------------
        //-- sort
        //----------------------------------------------------------------------
        Sort( &Tp[0], nb );

        //----------------------------------------------------------------------
        //-- make a list S/Sp
        //----------------------------------------------------------------------
        const char *names[] = { "S", "Sts","centering" };

        //-- first element: Satistic Value
        RVector<double> rT(1);
        rT[0] = T;

        //-- second element: Permutation Counts
        RVector<double> rTp(nb);
        for( size_t j=0; j < nb; ++j )
            rTp[j] = double( Tp[j] );

        //--
        C_Matrix<count_t> &M = xp.M;
        count_t ncross = 0;
        for(size_t i=0;i<M.rows;++i)
        {
            for(size_t j=0;j<M.cols;++j)
            {
                if(i!=j)
                {
                    ncross += M[i][j];
                }
            }
        }

        RVector<double> centering(1);
        centering[0] = double(ncross);

        RList L(names,sizeof(names)/sizeof(names[0]));
        L.set(0,rT);
        L.set(1,rTp);
        L.set(2,centering);
        return *L;

    }
    catch( const Exception &e )
    {
        Rprintf("*** wink_single_TS: %s\n", e.what());
    }
    catch(...)
    {
        Rprintf("*** unhanled exception in wink_single_TS\n");
    }
    return R_NilValue;
}


extern "C"
SEXP wink_coincmat(SEXP RN1, SEXP RN2, SEXP Ra, SEXP Rb, SEXP Rdelta)
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
        neurons              &xp    = shared_neurons();


        xp.initialize_correlations(N1,N2,a,b,delta);
        C_Matrix<count_t> &M = xp.M;

        //M.save_ascii("cm.dat");

        RMatrix<double>  RM(M.rows,M.cols);
        for(size_t i=0;i<M.rows;++i)
        {
            for(size_t j=0;j<M.cols;++j)
            {
                RM[j][i] = double(M[i][j]);
            }

        }

        return *RM;
    }
    catch( const Exception &e )
    {
        Rprintf("*** wink_coincmat: %s\n", e.what());
    }
    catch(...)
    {
        Rprintf("*** unhanled exception in wink_coincmat\n");
    }
    return R_NilValue;
}

extern "C"
SEXP wink_save_coincmat(SEXP RCM, SEXP Rfilename)
{
    try
    {

        const RMatrix<double> CM(RCM);
        const char *filename = R2String(Rfilename);

        FILE *fp = fopen(filename,"wb");
        if(!fp)
            throw Exception("can't open '%s'",filename);

        fprintf(fp,"%u\n", unsigned(CM.rows));
        fprintf(fp,"%u\n", unsigned(CM.cols));
        for(size_t i=0;i<CM.rows;++i)
        {
            for(size_t j=0;j<CM.cols;++j)
            {
                const RMatrix<double>::Column &col = CM[j];
                fprintf(fp,"%u\n",unsigned(col[i]));
            }
        }
        fclose(fp);

        return R_NilValue;
    }
    catch( const Exception &e )
    {
        Rprintf("*** wink_coincmat: %s\n", e.what());
    }
    catch(...)
    {
        Rprintf("*** unhanled exception in wink_coincmat\n");
    }
    return R_NilValue;

}

///////////////////////////////////////////////////////////
//
// CODE PERSO
//
///////////////////////////////////////////////////////////


/*  return lambda chapeau

 \On utilise les prepare directement sur le record (cad N[i]) pour eviter
 \ de faire 2 fois la meme boucle
 */
double lambdaChapeau(double a, double b, neuron& N){
    double res=0;
    size_t M = N.trials;
    double T = b-a;
    for(size_t i=0; i<M;i++){
        N[i].prepare(a,b);
        res+=N[i].iwin.length;
    }
    return res/(M*T);
}

extern "C"
SEXP lambdaChapeau1(SEXP Ra, SEXP Rb, SEXP RN1){
    RVector<double> result (1);
    //on lles convertis en double
    const double a=R2Scalar<double>(Ra);
    const double b=R2Scalar<double>(Rb);

    //Construit un neurone a partir des données de RN1
    const RMatrix<double> M1(RN1);
    RNeuron N1(M1);
    //N1.prepare_windows(a,b);
    const size_t M = N1.trials;
    const double T = b-a;
    result [0]=0;
    for(size_t i=0; i<M;i++){
        N1[i].prepare(a,b);
        result[0]+=N1[i].iwin.length;
    }
    result[0]/=(M*T);
    //__show_neuron(1,M1);
    return *result;
}

double m0Chapeau(double a, double b, double delta, neuron& N1, neuron& N2){
    double lbda1=lambdaChapeau(a,b,N1);
    double lbda2=lambdaChapeau(a,b,N2);
    double T = b-a;

    return lbda1*lbda2*(2.0*delta*T-pow(delta,2));
}

extern "C"
SEXP m0Chapeau1(SEXP Ra, SEXP Rb, SEXP Rdelta, SEXP RN1, SEXP RN2){
    //Intialision des donnes (Intrface R/C)
    RVector<double> result (1);
    const double a=R2Scalar<double>(Ra);
    const double b=R2Scalar<double>(Rb);
    const double delta=R2Scalar<double>(Rdelta);
    //Construit un neurone a partir des données de RN1
    const RMatrix<double> M1(RN1);
    RNeuron N1(M1);
    const RMatrix<double> M2(RN2);
    RNeuron N2(M2);


    double lbda1=lambdaChapeau(a,b,N1);
    double lbda2=lambdaChapeau(a,b,N2);
    double T = b-a;

    result[0]=lbda1*lbda2*(2.0*delta*T-pow(delta,2));

    return *result;
}

double vCarreChapeau (double a, double b, double delta, neuron& N1, neuron& N2){
    double lbda1=lambdaChapeau(a,b,N1);
    double lbda2=lambdaChapeau(a,b,N2);
    double T = b-a;

    return lbda1*lbda2*(2.0*delta*T-pow(delta,2)) +
    lbda1*lbda2*(lbda1+lbda2)*((2.0/3.0)*pow(delta,3)-(1.0/T)*pow(delta,4));
}

double sigmaCarreChapeau (double a, double b, double delta, neuron& N1, neuron& N2){
    double lbda1=lambdaChapeau(a,b,N1);
    double lbda2=lambdaChapeau(a,b,N2);
    double T = b-a;

    return lbda1*lbda2*(2*delta*T-pow(delta,2)) + (pow(lbda1,2)*lbda2 +
                                                   pow(lbda2,2)*lbda1)*(4*pow(delta,2)*T - (10.0/3.0)*pow(delta,3));
}

// Ici on doit faire le parallelisme ??
double mBar (double a, double b, const double delta, neuron& N1, neuron& N2){
    double res=0;
    size_t minTrials = std::min(N1.trials,N2.trials);
    for(size_t i=0;i<minTrials;i++){
        N1[i].prepare(a,b);
        N2[i].prepare(a,b);
        res+=N1[i].coincidences_with(N2[i],delta);
    }
    return res/minTrials;
}

//Fonction de repartition de la Gaussienne
double phi (double t){
    return 0.5*(1+erf(t/sqrt(2.0)));
}

double pValue(double a, double b, const double delta, neuron& N1, neuron& N2, int test){
    assert(test>=1 && test<=3);

    double m0=m0Chapeau(a,b,delta,N1,N2);
    double mBarre=mBar(a,b,delta,N1,N2);
    double v2=vCarreChapeau(a,b,delta,N1,N2);
    size_t M = std::min(N1.trials,N2.trials);

    double param=0;
    switch(test){
            //symetrical test
        case 1:
            param=fabs(mBarre-m0)/(sqrt(v2/M));
            Rprintf("param=%g mBarre=%g m0=%g v2=%g\n",param,mBarre,m0,v2);
            return 2.0-2.0*phi(param);
            //unilateral upper value
        case 2:
            param=(mBarre-m0)/(sqrt(v2/M));
            return 1.0-phi(param);
            //unilateral lower value
        case 3:
            param=(m0-mBarre)/(sqrt(v2/M));
            return 1.0-phi(param);
    }
}

//Verssion R
extern "C"
SEXP pValue1(SEXP Ra, SEXP Rb, SEXP Rdelta, SEXP RN1, SEXP RN2, SEXP Rtest){
    //Intialision des donnes (Intrface R/C)
    RVector<double> result (1);
    const double a=R2Scalar<double>(Ra);
    const double b=R2Scalar<double>(Rb);
    const double delta=R2Scalar<double>(Rdelta);
    const int test = R2Scalar<int>(Rtest);
    //Construit un neurone a partir des données de RN1
    const RMatrix<double> M1(RN1);
    RNeuron N1(M1);
    const RMatrix<double> M2(RN2);
    RNeuron N2(M2);

    //Données
    double m0=m0Chapeau(a,b,delta,N1,N2);
    double mBarre=mBar(a,b,delta,N1,N2);
    double v2=vCarreChapeau(a,b,delta,N1,N2);
    size_t M = std::min(N1.trials,N2.trials);

    //Les differents tests
    double param=0;
    switch(test){
            //symetrical test
        case 1:
            param=fabs(mBarre-m0)/(sqrt(v2/M));
            result[0] = 2-2*erf(param);
            break;
            //unilateral upper value
        case 2:
            param=(mBarre-m0)/(sqrt(v2/M));
            result[0] = 1-erf(param);
            break;
            //unilateral lower value
        case 3:
            param=(m0-mBarre)/(sqrt(v2/M));
            result[0] = 1-erf(param);
            break;
    }
    return *result;

}


//Calcul toutes les p valeurs pour différents
extern "C"
SEXP pValues(SEXP RMat, SEXP Rdelta, SEXP RN1, SEXP RN2, SEXP Rtest){
    const RMatrix<double> Mat(RMat);
    const size_t taille = Mat.cols;
    RVector<double> result(taille);

    const double delta=R2Scalar<double>(Rdelta);
    const int test = R2Scalar<int>(Rtest);
    //Construit un neurone a partir des données de RN1
    const RMatrix<double> M1(RN1);
    RNeuron N1(M1);
    const RMatrix<double> M2(RN2);
    RNeuron N2(M2);

    for (size_t i=0; i<taille;i++){
        const double a = Mat[i][0];
        const double b = Mat[i][1];
        result[i]=pValue(a,b,delta,N1,N2,test);
        Rprintf("i=%u a=%g b=%g p=%g\n",i,a,b,result[i]);
        
    }
    return *result;
}

////////////////////////////////////////////////////////////////////////////////
//
// Parallel Code
//
////////////////////////////////////////////////////////////////////////////////



extern "C"
SEXP pValuesPara(SEXP RMat, SEXP Rdelta, SEXP RN1, SEXP RN2, SEXP Rtest, SEXP RNumThreads)
{
    //__________________________________________________________________________
    //
    // Parse Arguments
    //__________________________________________________________________________

    //-- matrix of intervals a,b
    const RMatrix<double> Mat(RMat);
    const size_t taille = Mat.cols;

    //-- detection range
    const double delta = R2Scalar<double>(Rdelta);

    //-- kind of test
    const int    test  = R2Scalar<int>(Rtest);

    //-- Construit un neurone a partir des données de RN1
    const RMatrix<double> M1(RN1);
    RNeuron N1(M1);

    //-- Construit un neurone a partir des données de RN2
    const RMatrix<double> M2(RN2);
    RNeuron N2(M2);

    //-- Read

    //-- prepare output
    RVector<double> result(taille);


    for (size_t i=0; i<taille;i++){
        const double a = Mat[i][0];
        const double b = Mat[i][1];
        result[i]=pValue(a,b,delta,N1,N2,test);
        Rprintf("i=%u a=%g b=%g p=%g\n",i,a,b,result[i]);
        
    }
    return *result;
}



