#ifndef NC_CPWFN_INCLUDED
#define NC_CPWFN_INCLUDED 1


#include "train.hpp"
#include "yocto/container/tuple.hpp"
#include "yocto/bitwise.hpp"
#include "yocto/sequence/vector.hpp"

//! a coordinate (tau,value)
YOCTO_PAIR_DECL(Coord,Unit,tau,Unit,value);
YOCTO_PAIR_END();
YOCTO_SUPPORT_C_STYLE_OPS(Coord);

//! a Constant PieceWise function
/**
 - if tau <= tau[1], return foot
 - if tau[j]< tau <= tau[j+1] return value[j]
 - if tau[size()] < tau, return value[size()]
 */
class CPW_Function : public Object
{
public:
    //! a function with an intial capacity
    explicit CPW_Function(const Real usr_scale) throw();
    
    virtual ~CPW_Function() throw();

    Unit foot; //!< value before first point

    //! insert any new coordinate
    void insert(const Unit a_tau, const Unit a_value);

    //! number of coordinates
    size_t size() const throw();

    //! access operator in [1..size()]
    const Coord & operator[](const size_t indx) const throw();

    //! Coord[1], size()>0
    const Coord & front() const throw();

    //! Coord[size], size()>0
    const Coord & back() const throw();

    //! raw value for a given delta
    /**
     cost: less than train.size() comparison, train.size() temporary memory
     */
    void buildFrom(const RArray<Unit> &train,
                   const Unit          deltaUnit,
                   const bool          cleanUp=true);

    //! save in units
    void saveTo(const char *filename) const;


    void shiftBy(const Unit deltaUnit) throw();
    void copyAndShift(const CPW_Function &fn,
                      const Unit          deltaUnit);

    //! copy constructor, with shift
    CPW_Function(const CPW_Function &fn,
                 const Unit          deltaUnit);

    //! reset all
    void clear() throw();

    //! remove empty intervals
    void removeEmptyIntervals() throw();

    //! compute product by fusion
    /**
     cost: 2*min(lhs.size(),rhs.size()) comparisons...
     */
    void product(const CPW_Function &lhs, const CPW_Function &rhs);

    //! product
    CPW_Function(const CPW_Function &lhs, const CPW_Function &rhs);

    //! integrate with tauLo<=tauHi
    Real integrate(const Unit tauLo, const Unit tauHi) const throw();


private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(CPW_Function);
    vector<Coord> coords;
    //! find j such that tau[j] < tau < tau[size()]
    size_t find_index_for( const Unit tau ) const throw();
};

#endif
