#include "step-function.hpp"


int CompareCoords(const Coord &lhs, const Coord &rhs) throw()
{
    return __compare(lhs.x,rhs.x);
}

StepFunction:: ~StepFunction() throw()
{
}


size_t StepFunction::size() const throw()
{
    return coords.size();
}

StepFunction:: StepFunction(size_t n) :
coords(n,as_capacity)
{

}


void StepFunction:: insert(const double x, const double y)
{
    // append the coordinate
    {
        const Coord C(x,y);
        coords.push_back(C);
    }
    
    //check its place
    for(size_t j=coords.size(),jm=j-1;jm>0;--j,--jm)
    {
        Coord &Cm = coords[jm];
        Coord &C  = coords[j];
        if(CompareCoords(Cm, C)>0)
        {
            bswap(Cm,C);
        }
        else
        {
            break;
        }
    }
   
}



size_t StepFunction:: find_index(const double x) const throw()
{
    const size_t n = coords.size();
    assert(n>0);
    const Coord &C0 = coords.front();
    
    return 0;
}
