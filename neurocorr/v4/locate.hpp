#ifndef NC_LOCATE_INCLUDED
#define NC_LOCATE_INCLUDED 1

#include "types.hpp"

struct Locate
{
    //! locate the number of points lying in ]tauStart,tauFinal], and modify the starting value
    static size_t IndicesWithin_( const UArray &tau, const Unit tauStart, const Unit tauFinal, size_t &starting ) throw();

    //! locate the number of points lying in ]tauStart,tauFinal], and modify the starting value
    /**
     optimised version.
     */
    static size_t IndicesWithin(  const UArray &tau, const Unit tauStart, const Unit tauFinal, size_t &starting ) throw();
};

#endif
