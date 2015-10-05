#ifndef NC_BOX_INCLUDED
#define NC_BOX_INCLUDED 1

#include "phi.hpp"
#include "yocto/associative/map.hpp"

class   Box;
typedef slots_of<Box> _Boxes;


class Box
{
public:
    typedef int              Kind;
    typedef map<Kind,size_t> KindDB;

    const   size_t   trial;     //!< associated trial
    const   Unit     tauStart;  //!< tau start
    const   Unit     tauFinal;  //!< tau final
    const   Kind     kind;      //!< label
    mutable size_t   indx;      //!< index for collecting data

    explicit Box(const size_t indx,
                 const Unit   ini,
                 const Unit   end,
                 const Kind   aka=0) throw();
    virtual ~Box() throw();
    Box(const Box &) throw();


    //! extract the sorted list of times within the box in the train...
    //void extract( UList &Tau, const Train &train  ) const;

    
    inline friend std::ostream & operator<<(std::ostream &os, const Box &b )
    {
        os << "[@" << b.trial << ":" << b.tauStart << "->" << b.tauFinal << "|$" << b.kind << "]";
        return os;
    }

private:
    YOCTO_DISABLE_ASSIGN(Box);
};


#endif
