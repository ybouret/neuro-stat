#ifndef NC_TRAIN_INCLUDED
#define NC_TRAIN_INCLUDED 1

#include "types.hpp"
#include "yocto/sequence/vector.hpp"

typedef UVector _Train;

//! a vector of 1..size() units
class Train : public _Train
{
public:
    //! empty train
    explicit Train() throw();

    //! capacity only !
    explicit Train(size_t n);

    //! release memory
    virtual ~Train() throw();

    //! copy if needed
    Train(const Train &other);


    //! find indices such that tauStart<train[i]<=tauFinal
    size_t locateIndicesWithin_( const Unit tauStart, const Unit tauFinal, size_t &starting ) const throw();

    //! find indices such that tauStart<train[i]<=tauFinal
    size_t locateIndicesWithin( const Unit tauStart, const Unit tauFinal, size_t &starting ) const throw();


private:
    YOCTO_DISABLE_ASSIGN(Train);
};


#endif
