#ifndef NC_LOCATE_INCLUDED
#define NC_LOCATE_INCLUDED 1

#include "types.hpp"

struct Locate
{
    static size_t IndicesWithin_( const UArray &tau, const Unit tauStart, const Unit tauFinal, size_t &starting ) throw();
    static size_t IndicesWithin(  const UArray &tau, const Unit tauStart, const Unit tauFinal, size_t &starting ) throw();
};

#endif
