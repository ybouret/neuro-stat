#include "../pyck/R++.hpp"
#include "../pyck/exception.hpp"
#include "../pyck/wtime.hpp"
#include "../wink/neurons.hpp"

using namespace wink;

extern "C" SEXP wink_version()
{
    Rprintf("0.1 Compiled on %s\n", __DATE__);
    return R_NilValue;
}



namespace {
    class RNeuron : public RMatrix<double>, public neuron
    {
    public:
        explicit RNeuron( SEXP Rmat ) :
        RMatrix<double>(Rmat),
        neuron(rows, col2dat(cols) )
        {
            const RMatrix<double> &self = *this;
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
    
    static DefaultUniformGenerator shared_ran;
}

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

extern "C"
SEXP wink_true_coincidences( SEXP RN1, SEXP RN2, SEXP RI, SEXP Rdelta)
{
    
    try
    {
        //-- parse arguments
        RNeuron         N1(RN1);
        RNeuron         N2(RN2);
        RIntervals      intervals(RI);
        const double    delta = R2<double>(Rdelta);
        
        const size_t num_intervals = intervals.cols;
        
        //-- prepare answer
        RVector<double> ans(num_intervals);
        neurons         xp;
        for(size_t i=0; i < num_intervals;++i)
        {
            const double a = intervals[i][0];
            const double b = intervals[i][1];
            ans[i] = double(xp.true_coincidences(N1, N2, a, b, delta));
            Rprintf("%g -> %g: %g\n", a, b, ans[i]);
        }
        
        //-- done
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