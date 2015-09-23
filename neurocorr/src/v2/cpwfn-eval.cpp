#include "cpwfn.hpp"

namespace
{
    static inline
    size_t FindValueIndex(const Unit          tau,
                          const array<Coord> &coords,
                          size_t jlo,
                          size_t jup) throw()
    {
        assert(coords.size()>=2);
        assert(jlo>0);
        assert(jup  <= coords.size());
        assert(tau  >  coords[1].tau);
        assert(tau <=  coords[coords.size()].tau);
        while(jup-jlo>1)
        {
            const size_t jmid = (jlo+jup)>>1;
            const Unit   tmid = coords[jmid].tau;
            if(tau<=tmid)
            {
                jup = jmid;
            }
            else
            {
                jlo = jmid;
            }
        }
        assert(tau >coords[jlo].tau);
        assert(tau<=coords[jup].tau);
        return jlo;
    }

}

Unit  CPW_Function:: operator()(const Unit tau) const throw()
{
    const size_t N = coords.size();

    //__________________________________________________________________________
    //
    // Low sizes trivial cases
    //__________________________________________________________________________
    switch(N)
    {
        case 0:  return foot;
        case 1:
        {
            const Coord &C = coords.front();
            return (tau<=C.tau) ? foot : C.value;
        }
        default:  break;
    }

    //__________________________________________________________________________
    //
    // at least one interval
    //__________________________________________________________________________
    assert(N>=2);


    const Coord &C1   = coords.front();
    const Unit   tau1 = C1.tau;
    if(tau<=tau1)
    {
        return foot;
    }

    const Coord &CN   = coords.back();
    const Unit   tauN = CN.tau;
    if(tau>tauN)
    {
        return CN.value;
    }


    //__________________________________________________________________________
    //
    // Now it's somewhere in between..
    //__________________________________________________________________________
    const size_t j = FindValueIndex(tau,coords, 1, N);

    return coords[j].value;
}


Unit  CPW_Function:: _sumValuesAtOrdered( const RArray<Unit> &Tau ) const throw()
{
    Unit         ans= 0;
    const size_t np = Tau.size();

    for(size_t i=0;i<np;++i)
    {
        ans += (*this)(Tau[i]);
    }

    return ans;
}


Unit  CPW_Function:: sumValuesAtOrdered( const RArray<Unit> &Tau ) const throw()
{
    Unit ans=0;


    return ans;
}

