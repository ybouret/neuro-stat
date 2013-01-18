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
            Rprintf("WINK: Neuron: #trials=%u, #max_tops=%u\n", trials, length );
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

extern "C"
SEXP wink_bootstrap(SEXP RN1, SEXP RN2, SEXP RI, SEXP Rdelta, SEXP RB, SEXP Ropt)
{
    try
    {
        //----------------------------------------------------------------------
        //-- check option
        //----------------------------------------------------------------------
        const char *option = CHAR(STRING_ELT(Ropt,0));
        if(!option) throw Exception("NULL option");
        
        bootstrap_method kind = bootstrap_perm;
        bool             safe = false;
        if(strcmp(option,"perm")==0)
        {
            kind = bootstrap_perm;
            safe = true;
        }
        
        if(strcmp(option,"repl")==0)
        {
            kind = bootstrap_repl;
            safe = true;
        }
        if(!safe)
            throw Exception("Unknown option '%s'", option);
        
        //----------------------------------------------------------------------
        //-- parse arguments
        //----------------------------------------------------------------------
        RNeuron         N1(RN1);
        RNeuron         N2(RN2);
        RIntervals      intervals(RI);
        const double    delta         = R2<double>(Rdelta);
        const size_t    nb            = R2<int>(RB);
        if(nb<=0)
            throw Exception("Invalid B");
        const size_t    num_intervals = intervals.cols;
    
        Rprintf("\tWINK: #intervals  = %u\n", unsigned(num_intervals));
        Rprintf("\tWINK: #bootstraps = %u\n", unsigned(nb));

        //----------------------------------------------------------------------
        //-- prepare answer
        //-- first row: alpha_minus
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
            xp.bootstrap(Bcoinc, kind, N1, N2, delta);
            
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

