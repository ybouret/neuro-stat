#ifndef NC_STEP_FUNCTION_INCLUDED
#define NC_STEP_FUNCTION_INCLUDED 1

#include "yocto/sequence/vector.hpp"
#include "yocto/math/v2d.hpp"
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
    explicit StepFunction(size_t n);
    
    size_t size() const throw();
    
    void insert(const double t, const double v);
    
    //! find location of t if size()>0
    /**
     - if(j<=0), t is before fist point (t<t[1])
     - if(j>=size()), t is after last point
     - otherwise, t[j] < x <= t[j+1]
     */
    size_t find_index(const double x) const throw();
    
    
private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(StepFunction);
    CoordVector coords; // assume sorted coordinates
};

#endif
