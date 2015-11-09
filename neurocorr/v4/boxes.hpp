#ifndef NC_BOXES_INCLUDED
#define NC_BOXES_INCLUDED 1

#include "box.hpp"

enum Grouping
{
    GroupByKind,
    GroupByBox
};

//! base class for Boxes
typedef vector<Box> _Boxes;

//! an vector of boxes
class Boxes : public Converter, public _Boxes
{
public:
    //! destructor
    virtual ~Boxes() throw();

    //! prepare some boxes from a matrix of Real boxes
    /**
     \param usrScale the time scaling for Converter
     \param boxes a matrix with #colums=#boxes and at least 4 rows
     */
    explicit Boxes(const Real             usrScale ,
                   const matrix_of<Real> &boxes);

    //! which matrix to write, return the number or matrices to allocate
    size_t assignIndices(const Grouping mode) const;


private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(Boxes);
};

#endif

