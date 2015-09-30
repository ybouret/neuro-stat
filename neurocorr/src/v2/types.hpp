#ifndef NC_TYPES_INCLUDED
#define NC_TYPES_INCLUDED 1

#include "yocto/sequence/list.hpp"
#include "yocto/R/IR.hpp"
#include "yocto/sequence/slots.hpp"

using namespace yocto;

typedef unit_t Unit;
typedef double Real;


//! base class for integer/real convertsion
class Object : public object
{
public:
    const Real scale;
    
    //! init with usr_scale>0
    explicit Object(const Real usr_scale) throw();
    
    //! clean up
    virtual ~Object() throw();
    
    //! copy scale
    Object(const Object &other) throw();
    
    //! conversion
    Unit toUnit(const Real t) const throw();
    Real toReal(const Unit u) const throw();

    static int CompareUnit(const Unit &lhs, const Unit &rhs) throw();

private:
    YOCTO_DISABLE_ASSIGN(Object);
};

typedef list<Unit> UList;
typedef Matrix<Real> *      MatrixPtr;
typedef slots_of<MatrixPtr> Matrices;

class CMatrices : public CoreMatrix, public Matrices
{
public:
    explicit CMatrices(const size_t nm, const size_t nr, const size_t nc) :
    CoreMatrix(),
    Matrices(nm),
    M(nm)
    {
        assert(nr>0);
        assert(nc>0);
        assert(nm>0);
        this->setDimensions(nr,nc);
        // allocate matrice and register them
        for(size_t i=0;i<nm;++i)
        {
            M.append<size_t,size_t>(nr,nm);
            this->push_back( &M[i] );
        }
    }

    virtual ~CMatrices() throw()
    {
    }
    


private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(CMatrices);
    slots_of< CMatrix<Real> > M;
};


#endif

