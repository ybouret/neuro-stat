//! \file
/**
 \brief fast RNG
 random number generators for fast permutations,
 possibly in parallel
 */

#ifndef WINK_RAND32_INCLUDED
#define WINK_RAND32_INCLUDED 1

#include <stddef.h>
#include <stdint.h>
#include <assert.h>

namespace wink 
{
    
    //! Marsaglia's 32 bits random number generators
    class rand32
    {
    public:
        uint32_t z,w,jsr,jcong,a,b;
        uint32_t x,y,bro;
        uint8_t  c;
        uint32_t t[256];
        
        explicit rand32() throw();
        virtual ~rand32() throw();
        
        typedef uint32_t (rand32::*generator)();
        uint32_t mwc()   throw();
        uint32_t shr3()  throw();
        uint32_t cong()  throw();
        uint32_t fib()   throw();
        uint32_t kiss()  throw();
        uint32_t lfib4() throw();
        uint32_t swb()   throw();
        
        void settable( uint32_t i1, uint32_t i2, uint32_t i3, uint32_t i4, uint32_t i5, uint32_t i6 ) throw();
        void reset( uint32_t s ) throw();
        
        //! make a random 0 or 1
        template <typename T>
        static inline T to_bit( uint32_t u ) throw() 
        { 
            static const uint32_t threshold = uint32_t(1) << 31;
            static const T __zero(0);
            static const T __one(1);
            return u >= threshold ? __one : __zero;
        }
        
        
        //! convert to 0:1 exclusive
        static inline double to_double( uint32_t u ) throw()
        {
            return (0.5+double(u))/4294967296.0;
        }
        
        //! convert to 0:1 exclusive
        static inline double to_float( uint32_t u ) throw()
        {
            return (0.5f+float(u))/4294967296.0f;
        }
        
        static uint32_t ih32( uint32_t ) throw();
        
        //! should print 7 '0'
        void test() throw(); 
        
    private:
        rand32( const rand32 & );
        rand32&operator=(const rand32 &);
    };
    
    //! integer to type mapping.
	template <int v>
	struct int2type {
		enum
		{
		    value = v //!< a different class for each v.
		};
	};
    
    //! interface to a 32 bits uniform generator
    class urand32 
    {
    public:
        virtual ~urand32() throw();
        
        virtual uint32_t next() throw() = 0;
        virtual void     seed(uint32_t) throw() = 0;
        
        template <typename T> T get() throw(); //!< valid for uin32_t, float, double
        
        inline double alea() throw()
        {
            return rand32::to_double(next());
        }
        
        template <typename T>
        inline T full() throw()
        {
            return __full<T>( int2type<(sizeof(T)>sizeof(uint32_t))>() ); 
        }
        
        inline size_t less_than( size_t n ) throw() 
        {
            return full<size_t>() % (++n);
        }
        
        
        template <typename T>
        inline void shuffle( T *a, size_t n ) throw()
        {
            assert(!(NULL==a&&n>0));
            if( n > 1 )
            {
                for( size_t i=n-1;i>0;--i)
                {
                    const size_t j   = less_than(i); assert(j<=i);
                    const T tmp(a[i]);
                    a[i] = a[j];
                    a[j] = tmp;
                }
            }
        }
        
        void fill_array( double a, double b, double *x, size_t n ) throw();        
        
    protected:
        explicit urand32() throw();
        
    private:
        urand32(const urand32 &);
        urand32&operator=(const urand32 &);
        template <typename T>
        inline T __full( int2type<false> ) throw()
        {
            assert(sizeof(T)<=sizeof(uint32_t));
            return T(next());
        }
        
        template <typename T>
        inline T __full( int2type<true> ) throw()
        {
            assert(sizeof(T)>sizeof(uint32_t));
            T ans(0);
            for( size_t i=sizeof(T)/sizeof(uint32_t);i>0;--i)
            {
                ans <<= 32;
                ans |=  T(next());
            }
            return ans;
        }
        
    };
    
    //! generic random number generator
    template <rand32::generator G>
    class grand32 : public urand32
    {
    public:
        explicit grand32() throw() : urand32(), r() {}
        virtual ~grand32() throw() {}
        virtual uint32_t next() throw() { return (r.*G)(); }
        virtual void     seed(uint32_t s) throw() { r.reset(s); }
        
    private:
        rand32 r;
        grand32(const grand32 & );
        grand32&operator=(const grand32 &);
    };
    
    //==========================================================================
    // available random number generators
    //==========================================================================
    typedef grand32<&rand32::mwc>    rand32_mwc;
    typedef grand32<&rand32::shr3>   rand32_shr3;
    typedef grand32<&rand32::cong>   rand32_cong;
    typedef grand32<&rand32::fib>    rand32_fib;
    typedef grand32<&rand32::kiss>   rand32_kiss;
    typedef grand32<&rand32::lfib4>  rand32_lfib4;
    typedef grand32<&rand32::swb>    rand32_swb;
    
    
}

#endif
