#ifndef NC_BOX_INCLUDED
#define NC_BOX_INCLUDED 1

#include "types.hpp"
#include "phifn.hpp"

class Box
{
public:
    const size_t  trial;     //!< associated trial
    const Unit    tauStart;  //!< tau start
    const Unit    tauFinal;  //!< tau final
    const int     kind;      //!< label
    mutable UList Tau;       //!< local tau's
    
    explicit Box(const size_t indx,
                 const Unit   ini,
                 const Unit   end) throw();
    virtual ~Box() throw();
    Box(const Box &) throw();

    void extractTauFrom( const Train &train ) const;

    void computeFor(const PHI &Phi, vector<Unit> &b ) const;

    
private:
    YOCTO_DISABLE_ASSIGN(Box);
};

#endif
