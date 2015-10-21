#ifndef NC_BOXES_INCLUDED
#define NC_BOXES_INCLUDED 1

#include "box.hpp"
#include "yocto/math/matrix.hpp"

enum Grouping
{
    GroupByKind,
    GroupByBox
};

typedef vector<Box> _Boxes;

class Boxes : public Converter, public _Boxes
{
public:
    virtual ~Boxes() throw();
    explicit Boxes(const Real            usrScale ,
                   const matrix_of<Real> boxes);

    //! which matrix to write, return the number or matrices to allocate
    size_t assignIndices(const Grouping mode) const;


private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(Boxes);
};

#endif

