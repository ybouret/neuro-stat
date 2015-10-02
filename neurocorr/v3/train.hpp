#ifndef NC_TRAIN_INCLUDED
#define NC_TRAIN_INCLUDED 1

#include "types.hpp"


typedef CVector<Unit> _Train;

class Train : public object, public _Train
{
public:

    explicit Train(const size_t np);
    virtual ~Train() throw();

    void save(const char *filename) const;

    //! find indices of tauStart<=tau<=tauEnd, return the count, update the idxStart
    size_t findIndicesWithin(const Unit tauStart,
                             const Unit tauFinal,
                             size_t    &idxStart) const throw();

    size_t findIndicesWithin_(const Unit tauStart,
                              const Unit tauFinal,
                              size_t    &idxStart) const throw();
private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(Train);
};

#endif

