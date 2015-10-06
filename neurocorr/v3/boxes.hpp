#ifndef NC_BOXES_INCLUDED
#define NC_BOXES_INCLUDED 1

#include "box.hpp"
#include "yocto/bitwise.hpp"

enum Grouping
{
    GroupByKind,
    GroupByBox
};

//! Boxes
class Boxes : public Converter, public _Boxes
{
public:
    //! extracted from a R matrix
    /**
     \warning trials indices start from 1 in R data
     */
    explicit Boxes( const Real usr_scale, const Matrix<Real> &RBoxes);

    //just reserve memory
    explicit Boxes( const Real usr_scale, const size_t nboxes);

    virtual ~Boxes() throw();

    
    //! prepare boxes and return the number of needed Matrices
    size_t assignIndices(const Grouping mode) const;
    
private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(Boxes);
};



#endif
