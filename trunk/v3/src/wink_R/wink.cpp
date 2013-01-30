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

neurons & shared_neurons() throw()
{
    static neurons XP;
    return XP;
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




