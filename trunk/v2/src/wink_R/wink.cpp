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
    
    
    static inline void __show_neuron( int id, const RMatrix<double> &r )
    {
        Rprintf("\tWINK: Neuron %d | #trials=%4u | max_tops=%5u\n", id, unsigned(r.rows), unsigned(r.cols-1) );
    }
    
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
    
    static
    Mutex &shared_mutex()
    {
        static Mutex m;
        return m;
    }
    
    static
    neurons & shared_neurons()
    {
        static neurons n;
        return n;
    }
    
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
        
        PYCK_LOCK(shared_mutex());
        shared_neurons().ran.shuffle( &ans[0], n);
        
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
        const RMatrix<double> M1(RN1); __show_neuron(1,M1);
        RNeuron               N1(M1);
        const RMatrix<double> M2(RN2); __show_neuron(2,M2);
        RNeuron               N2(M2);
        RIntervals            intervals(RI);
        const double          delta = R2<double>(Rdelta);
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
        const RMatrix<double> M1(RN1); __show_neuron(1,M1);
        RNeuron               N1(M1);
        const RMatrix<double> M2(RN2); __show_neuron(2,M2);
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
SEXP wink_bootstrap_counts(SEXP RN1, SEXP RN2, SEXP RI, SEXP Rdelta, SEXP RB)
{
    try
    {
        Rprintf("\tWINK: Serial Bootstrap Code\n");
        
        //----------------------------------------------------------------------
        //-- parse arguments
        //----------------------------------------------------------------------
        const RMatrix<double> M1(RN1); __show_neuron(1,M1);
        RNeuron               N1(M1);
        const RMatrix<double> M2(RN2); __show_neuron(2,M2);
        RNeuron               N2(M2);
        RIntervals            intervals(RI);
        const double          delta         = R2<double>(Rdelta);
        const size_t          nb            = R2<int>(RB);
        const size_t          num_intervals = intervals.cols;
        
        Rprintf("\tWINK: #intervals  = %u\n", unsigned(num_intervals));
        Rprintf("\tWINK: #bootstraps = %u\n", unsigned(nb));
        
        
        //----------------------------------------------------------------------
        //-- prepare answer
        //-- first  row: count_minus
        //-- second row: count_plus
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
            
            //-- mix'em all, bootstrap kind
            xp.mix( statistic_H, Bcoinc, mix_boot, N1, N2, delta);
            
            //-- center
            for(size_t j=0; j < nb; ++j )
                Bcoinc[j] -= H;
            
            //-- evaluate counts
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

#include "../pyck/sort.hpp"
extern "C"
SEXP wink_single_boot(SEXP RN1, SEXP RN2, SEXP Ra, SEXP Rb, SEXP Rdelta, SEXP RB)
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
        const double          a     = R2<double>(Ra);
        const double          b     = R2<double>(Rb);
        const double          delta = R2<double>(Rdelta);
        const size_t          nb    = R2<int>(RB);
        neurons              &xp    = shared_neurons();
        
        
        PYCK_LOCK( shared_mutex() );
        //----------------------------------------------------------------------
        //-- initialize window
        //----------------------------------------------------------------------
        const count_t    H = xp.true_coincidences( statistic_H, N1, N2, a, b, delta);
        C_Array<count_t> Hc(nb);
        
        //----------------------------------------------------------------------
        //-- mix'em all, bootstrap kind
        //----------------------------------------------------------------------
        xp.mix( statistic_H, Hc, mix_boot, N1, N2, delta);
        
        //----------------------------------------------------------------------
        //-- center
        //----------------------------------------------------------------------
        for( size_t j=0; j < nb; ++j )
        {
            Hc[j] -= H;
        }
        
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
        
        //-- second element: Hc
        RVector<double> rHc(nb);
        for( size_t j=0; j < nb; ++j )
            rHc[j] = double( Hc[j] );
        
        //-- create the list
        SEXP L = 0, list_names=0;
        PROTECT( L = allocVector(VECSXP,2) );
        SET_VECTOR_ELT(L, 0, *rH);
        SET_VECTOR_ELT(L, 1, *rHc);
        
        //-- and set the fields name
        PROTECT(list_names = allocVector(STRSXP,2));
        for(size_t i = 0; i < 2; i++)
            SET_STRING_ELT(list_names,i,mkChar(names[i]));
        setAttrib(L, R_NamesSymbol, list_names);
        
        //-- done
        UNPROTECT(2); // list+names
        return L;
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
SEXP wink_single_perm(SEXP RN1, SEXP RN2, SEXP Ra, SEXP Rb, SEXP Rdelta, SEXP RB, SEXP Ropt)
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
        const double          a     = R2<double>(Ra);
        const double          b     = R2<double>(Rb);
        const double          delta = R2<double>(Rdelta);
        const size_t          nb    = R2<int>(RB);
        neurons              &xp    = shared_neurons();
        const statistic_value  S    = __check_stat_val(Ropt);

        
        PYCK_LOCK( shared_mutex() );
        //----------------------------------------------------------------------
        //-- initialize window
        //----------------------------------------------------------------------
        const count_t    T = xp.true_coincidences( S, N1, N2, a, b, delta);
        C_Array<count_t> Tp(nb);
        
        //----------------------------------------------------------------------
        //-- mix'em all, bootstrap kind
        //----------------------------------------------------------------------
        xp.mix( S, Tp, mix_perm, N1, N2, delta);
        
       
        
        //----------------------------------------------------------------------
        //-- sort
        //----------------------------------------------------------------------
        Sort( &Tp[0], nb );
        
        //----------------------------------------------------------------------
        //-- make a list H/Hc
        //----------------------------------------------------------------------
        const char *names[] = { "S", "Sp" };
        
        //-- first element: H
        RVector<double> rT(1);
        rT[0] = T;
        
        //-- second element: Hc
        RVector<double> rTp(nb);
        for( size_t j=0; j < nb; ++j )
            rTp[j] = double( Tp[j] );
        
        //-- create the list
        SEXP L = 0, list_names=0;
        PROTECT( L = allocVector(VECSXP,2) );
        SET_VECTOR_ELT(L, 0, *rT);
        SET_VECTOR_ELT(L, 1, *rTp);
        
        //-- and set the fields name
        PROTECT(list_names = allocVector(STRSXP,2));
        for(size_t i = 0; i < 2; i++)
            SET_STRING_ELT(list_names,i,mkChar(names[i]));
        setAttrib(L, R_NamesSymbol, list_names);
        
        //-- done
        UNPROTECT(2); // list+names
        return L;
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
        const size_t           B;
        C_Array<count_t>       Bcoinc;
        const mix_method       Bkind;
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
        B( args.B ),
        Bcoinc(B),
        Bkind( args.Bkind ),
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
                    const size_t Tcoinc = double(xp.true_coincidences(S, N1, N2, a, b, delta));
                    
                    //-- permutations
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
            const size_t     nb     = Bcoinc.size;
            try
            {
                for( size_t i=ini,j=0;j<num;++i,++j)
                {
                    const double a = intervals[i][0];
                    const double b = intervals[i][1];
                    
                    //-- initialize with true coincidences
                    const size_t H  = double(xp.true_coincidences( statistic_H, N1, N2, a, b, delta));
                    
                    //-- mix'em all, bootstrap kind
                    xp.mix( statistic_H, Bcoinc, mix_boot, N1, N2, delta);
                    
                    //-- center
                    for(size_t k=0; k < nb; ++k )
                        Bcoinc[k] -= H;
                    
                    //-- evaluate counts
                    xp.compute_counts(counts[i][0],counts[i][1],Bcoinc,H);
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
        WorkerBoot(const WorkerBoot &);
        WorkerBoot&operator=(const WorkerBoot &);
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
        RMatrix<double>        M1(RN1); __show_neuron(1,M1);
        RMatrix<double>        M2(RN2); __show_neuron(2,M2);
        RIntervals             intervals(RI);
        const double           delta = R2<double>(Rdelta);
        const size_t           B     = R2<int>(RB);
        
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
        Team<WorkerPerm> team(num_threads);
        WorkerArgs       args = { &M1, &M2, S, &intervals, delta, B, Bkind, &alpha, num_threads };
        for(size_t i=0;i<num_threads;++i)
        {
            args.thread_rank = i;
            team.enqueue( args );
        }
        team.finish(); //-- not necessary, called by destructor before returning
        
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
// Parallel Bootstrap
//
////////////////////////////////////////////////////////////////////////////////
extern "C"
SEXP wink_bootstrap_counts_par(SEXP RN1, SEXP RN2, SEXP RI, SEXP Rdelta, SEXP RB, SEXP RNumThreads)
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
        Rprintf("\tWINK: Parallel Bootstrap Code [%u thread%c]\n", unsigned(num_threads), num_threads>1 ? 's' : ' ' );
        
        RMatrix<double>        M1(RN1); __show_neuron(1,M1);
        RMatrix<double>        M2(RN2); __show_neuron(2,M2);
        RIntervals             intervals(RI);
        const double           delta         = R2<double>(Rdelta);
        const size_t           B             = R2<int>(RB);
        
        //----------------------------------------------------------------------
        //-- prepare answer
        //-- first  row: alpha_minus
        //-- second row: alpha_plus
        //----------------------------------------------------------------------
        const size_t num_intervals = intervals.cols;
        RMatrix<double> counts(2,num_intervals);
        
        Rprintf("\tWINK: #intervals  = %6u\n", unsigned(num_intervals));
        Rprintf("\tWINK: #bootstraps = %6u\n", unsigned(B));
        
        //----------------------------------------------------------------------
        //
        // Launching the team
        //
        //----------------------------------------------------------------------
        Team<WorkerBoot> team(num_threads);
        WorkerArgs       args = { &M1, &M2, statistic_H, &intervals, delta, B, mix_boot, &counts, num_threads };
        for(size_t i=0;i<num_threads;++i)
        {
            args.thread_rank = i;
            team.enqueue( args );
        }
        team.finish();
        
        //----------------------------------------------------------------------
        //
        // All done
        //
        //----------------------------------------------------------------------
        return *counts;
    }
    catch( const Exception &e )
    {
        Rprintf("*** wink_bootstrap_counts_par: %s\n", e.what());
    }
    catch(...)
    {
        Rprintf("*** unhanled exception in wink_bootstrap_counts_par\n");
    }
    return R_NilValue;
}



