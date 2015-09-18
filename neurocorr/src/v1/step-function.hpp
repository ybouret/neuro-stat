#ifndef NC_STEP_FUNCTION_INCLUDED
#define NC_STEP_FUNCTION_INCLUDED 1

#include "record.hpp"
#include "yocto/sequence/vector.hpp"
#include "yocto/comparator.hpp"
#include "yocto/container/tuple.hpp"

using namespace yocto;

YOCTO_PAIR_DECL(Coord, double, t, double, v);
YOCTO_PAIR_END();
YOCTO_SUPPORT_C_STYLE_OPS(Coord);

//typedef math::v2d<double> Coord;
typedef vector<Coord>     CoordVector;

int CompareCoords(const Coord &lhs, const Coord &rhs) throw();

//! Left-Continuous step function
class StepFunction
{
public:
    virtual ~StepFunction() throw();
    explicit StepFunction(size_t n=0);

    double foot; //!< initialized to 0, returned if size()<=0
    
    size_t size() const throw(); //! #coords
    
    void insert(const double t, const double v);
    
    //! find location of t if size()>0
    /**
     - if(j<=0), t is before fist point (t<=t[1])
     - if(j>=size()), t is after last point (t>t[size()])
     - otherwise, t[j] < t <= t[j+1]
     */
    size_t find_index(const double t) const throw();

    //! Direct computation for size()>0
    /**
     - if(t<=t[1]), headValue
     - if(t>t[size]), tailValue
     - otherwise, find the partition and return the value
     */
    double operator()(const double t) const throw();

    void buildFromFull( const Record &train, const double delta);

    const Coord & operator[](const size_t indx) const throw();

private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(StepFunction);
    CoordVector coords; // assume sorted coordinates

    //! find index for t>t[1] and t <=t[size]
    size_t find_index_(const double t) const throw();
};

#endif
