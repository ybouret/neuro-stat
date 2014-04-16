/**
 \file
 \brief drawing couple of indices for bootstrapping
 */
#ifndef WINK_DRAWING_INCLUDED
#define WINK_DRAWING_INCLUDED 1


#include "../pyck/pool.hpp"
#include "../pyck/list.hpp"
#include "../pyck/rand32.hpp"
#include "../pyck/c_array.hpp"
#include <iosfwd>

namespace wink
{
    
    //! couple of indices
    struct couple
    {
        size_t  first;
        size_t  second;
        couple *next;
        couple *prev;
        
        typedef ListOf<couple> list;
        typedef PoolOf<couple> pool;
        static void kill( couple * ) throw();
    };
    
    //! a drawing is a list of couples
    class drawing : public couple::list
    {
    public:
        explicit drawing() throw();
        virtual ~drawing() throw();
        
        //! remove all couple from current list
        void clear() throw();
        
        //! put n couples in cache
        void reserve( size_t n );
        
        //!identity drawing
        /**
         couple->first = [0..n-1]
         couple->second = couple->first
         */
        void identity( size_t n );
        
        //! drawing without replacement
        /**
         couple->first  = [0..n-1]
         couple->second = perm([0..n-1])
         */
        void permutation( size_t n, UniformGenerator &ran );
        
        //! drawing with replacement (marginal)
        /**
         couple->first  = random([0..n-1])
         couple->second = random([0..n-1])
         */
        void replacement( size_t n, UniformGenerator &ran ); 
        
        //! bootstrap
        /**
         couple->first = couple->second = random([0..n-1])
         */
        void bootstrap2(size_t n, UniformGenerator &ran );
        
        //! trial shuffle
        /**
         couple->first  = random([0..n-1])
         couple->second = random([0..n-1]) != couple->first
         */
        void trial_shuffling(size_t n, UniformGenerator &ran );
        
        //! display, for debugging
        friend std::ostream & operator<<( std::ostream &, const drawing & );
        
    private:
        couple *query();
        couple::pool    pool;
        drawing(const drawing &);
        drawing&operator=(const drawing &);
        
    public:
        C_Array<size_t> arr; //!< for fast permutations
    };
    
}

#endif
