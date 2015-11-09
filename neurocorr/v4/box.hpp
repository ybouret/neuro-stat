#ifndef NC_BOX_INCLUDED
#define NC_BOX_INCLUDED 1

#include "types.hpp"

//! a temporal box
class Box
{
public:
    typedef int      Kind; //!< kind of behavior

    const   size_t   trial;     //!< associated trial
    const   Unit     tauStart;  //!< tau start
    const   Unit     tauFinal;  //!< tau final
    const   Kind     kind;      //!< behavior label
    mutable size_t   indx;      //!< index for collecting data

    //! build a box
    /**
     \param iTrial trial index
     \param ini    start time in time unit
     \param end    final time in time unit
     \param aka    which kind of behavior
     */
    explicit Box(const size_t iTrial,
                 const Unit   ini,
                 const Unit   end,
                 const Kind   aka) throw();

    //! desctructor
    virtual ~Box() throw();

    //! copy constructor
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
