#include "step-function.hpp"


int CompareCoords(const Coord &lhs, const Coord &rhs) throw()
{
    return __compare(lhs.t,rhs.t);
}

StepFunction:: ~StepFunction() throw()
{
}


size_t StepFunction::size() const throw()
{
    return coords.size();
}

StepFunction:: StepFunction(size_t n) :
head(0),
tail(0),
coords(n,as_capacity)
{

}


void StepFunction:: insert(const double t, const double v)
{
    // append the coordinate
    {
        const Coord C(t,v);
        coords.push_back(C);
    }
    
    //check its place
    for(size_t j=coords.size(),jm=j-1;jm>0;--j,--jm)
    {
        Coord &Cm = coords[jm];
        Coord &C  = coords[j];
        if(CompareCoords(Cm,C)>0)
        {
            bswap(Cm,C);
        }
        else
        {
            break;
        }
    }
   
}


size_t StepFunction:: find_index_(const double t) const throw()
{
    assert(t>coords.front().t);
    assert(t<=coords.back().t);
    size_t jlo = 1;
    size_t jhi = size();
    while(jhi-jlo>1)
    {
        const size_t jmid = (jlo+jhi)>>1;
        const double tmid = coords[jmid].t;
        if(t<=tmid)
        {
            jhi = jmid;
        }
        else
        {
            if(t>tmid)
            {
                jlo = jmid;
            }
            else
            {
                return jmid; // special numeric case, t==tmid
            }
        }
    }
    return jlo;

}


size_t StepFunction:: find_index(const double t) const throw()
{
    const size_t n = coords.size();
    assert(n>0);
    const Coord &C1 = coords.front();
    if(t<=C1.t)
    {

        return 0;
    }
    else
    {
        const Coord &Cn = coords.front();
        if(t>Cn.t)
        {
            return n;
        }
        else
        {
            return find_index_(t);
        }
    }

}

double StepFunction:: operator()(const double t) const throw()
{
    if(t<=coords.front().t)
    {
        return head;
    }
    else
    {
        if(t>coords.back().t)
        {
            return tail;
        }
        else
        {
            return coords[ find_index_(t) ].v;
        }
    }
}
