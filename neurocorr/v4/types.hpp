#ifndef NC_TYPES_INCLUDED
#define NC_TYPES_INCLUDED

#include "yocto/threading/crew.hpp"
#include "yocto/container/matrices.hpp"

using namespace yocto;


typedef int64_t Unit;
typedef double  Real;

inline Unit UnitAbs(const Unit x)  throw()
{
    return (x < 0) ? -x : x;
}

//! convertion Real<->Unit
class Converter
{
public:
    explicit Converter(const Real usr_scale) throw();
    virtual ~Converter() throw();

    const Real scale;

    //! convention is 'to nearest'
    Unit toUnit(const Real x) const throw();

    //! u/scale
    Real toReal(const Unit u) const throw();

private:
    YOCTO_DISABLE_ASSIGN(Converter);
};

typedef array<Unit>             UArray;
typedef vector<Unit>            UVector;

typedef matrices<Unit,matrix>   UMatrices;

inline
bool areEqualMatrix(const matrix_of<Unit> &lhs, const matrix_of<Unit> &rhs) throw()
{
    assert(lhs.rows==rhs.rows);
    assert(rhs.cols==rhs.cols);
    assert(lhs.items==rhs.items);
    const size_t items = lhs.items;
    for(size_t i=0;i<items;++i)
    {
        if( lhs.fetch(i) != rhs.fetch(i) )
        {
            return false;
        }
    }
    return true;
}

inline
bool areEqualMatrices(const matrices_of<Unit> &lhs, const matrices_of<Unit> &rhs )
{
    assert(lhs.count==rhs.count);
    const size_t count = lhs.count;
    for(size_t i=1;i<=count;++i)
    {
        if( !areEqualMatrix(lhs[i],rhs[i]) )
            return false;
    }
    return true;
}



#endif

