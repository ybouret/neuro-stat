/**
 \file
 \brief Info about a neurophys. trial
 */

#ifndef WINK_NEURO_TRIALS_INCLUDED
#define WINK_NEURO_TRIALS_INCLUDED 1

#include "wink-iwindow.hpp"

namespace wink 
{
    
    //! holds data and extra memory
    class neuro_trials
    {
    public:
        //! constructor: allocate memory from a raw experiment
        /**
         \param data
         \param nr number of records => num
         \param nc number of columns => top = nc-1 (first column=count)
         */
        neuro_trials( const double *data, const size_t nr, const size_t nc );
        ~neuro_trials() throw();
        
        
        //! one row + one associated window
        class record
        {
        public:
            const double *X; //!< one trial X[0]=Nx, X[1..Nx]
            iwindow       W; //!< associated window
            record() throw();
            ~record() throw();
            
        private:
            record( const record & );
            record &operator=(const record & );
        };
        
        const size_t num;  //!< #num records = nr
        const size_t top;  //!< max valid index: nc-1 
        record      *rec;  //!< array of num records
        
        //! prepare the windows for all the records
        void prepare_windows( double a, double b ) throw();
        
        
        void display() const; //!< to debug...
        
    private:
        neuro_trials( const neuro_trials & );
        neuro_trials&operator=(const neuro_trials & );
    };

    
}

#endif
