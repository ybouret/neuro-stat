#ifndef NC_STEP_FUNCTION_INCLUDED
#define NC_STEP_FUNCTION_INCLUDED 1

#include "yocto/sequence/vector.hpp"
#include "yocto/math/v2d.hpp"
#include "yocto/comparator.hpp"

using namespace yocto;

typedef math::v2d<double> Coord;
typedef vector<Coord>     CoordVector;

int CompareCoords(const Coord &lhs, const Coord &rhs) throw();

//! Left-Continuous step function
class StepFunction
{
public:
    virtual ~StepFunction() throw();
    explicit StepFunction(size_t n);
    
    size_t size() const throw();
    
    void insert(const double x, const double y);
    
    //! find location of x if size()>0
    /**
     - if(j<=0), x is before fist point
     - if(j>=size()), x is after last point
     - otherwise, x[j] < x <= x[j+1]
     */
    size_t find_index(const double x) const throw();
    
    
private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(StepFunction);
    CoordVector coords; // assume sorted coordinates
};

#endif
