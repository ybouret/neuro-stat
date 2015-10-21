#ifndef NC_BOX_INCLUDED
#define NC_BOX_INCLUDED 1

#include "types.hpp"

class Box
{
public:
    typedef int              Kind;

    const   size_t   trial;     //!< associated trial
    const   Unit     tauStart;  //!< tau start
    const   Unit     tauFinal;  //!< tau final
    const   Kind     kind;      //!< label
    mutable size_t   indx;      //!< index for collecting data

    explicit Box(const size_t iTrial,
                 const Unit   ini,
                 const Unit   end,
                 const Kind   aka) throw();
    virtual ~Box() throw();
    Box(const Box &) throw();

    inline friend std::ostream & operator<<(std::ostream &os, const Box &b )
    {
        os << "[@" << b.trial << ":" << b.tauStart << "->" << b.tauFinal << "|$" << b.kind << "]";
        return os;
    }

private:
    YOCTO_DISABLE_ASSIGN(Box);
};

YOCTO_SUPPORT_NO_DESTRUCT(Box);

#endif
