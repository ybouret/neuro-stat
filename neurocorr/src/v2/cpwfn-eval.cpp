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


Unit  CPW_Function:: _sumValuesAtOrdered(const UList &Tau) const throw()
{
    Unit                  ans = 0;
    const size_t          np  = Tau.size();
    UList::const_iterator it  = Tau.begin();
    for(size_t i=np;i>0;--i,++it)
    {
        ans += (*this)(*it);
    }
    return ans;
}



Unit  CPW_Function:: sumValuesAtOrdered( const RArray<Unit> &Tau ) const throw()
{
    //__________________________________________________________________________
    //
    // Get number of Tau values
    //__________________________________________________________________________
    const size_t M = Tau.size();
    if(M<=0) return 0;

    //__________________________________________________________________________
    //
    // M>0, let's start to work
    //__________________________________________________________________________
    const size_t N   = coords.size();
    Unit         ans = 0;
    switch(N)
    {
            //__________________________________________________________________
            //
            // Trivial case: constant function
            //__________________________________________________________________
        case 0: return foot*M;

            //__________________________________________________________________
            //
            // Trivial case: 1 point function
            //__________________________________________________________________
        case 1: {
            // easy way, linear sear
            const Coord &C   = coords.front();
            const Unit   tau = C.tau;
            size_t       i   = 0;
            while(i<M)
            {
                if(Tau[i]>tau) break;
                ++i;
            }

            ans +=  i    * foot;
            ans += (M-i) * C.value;
            return ans;
        }

        default:
            break;
    }
    
    assert(N>=2);
    const Coord &C1   = coords.front();
    const Unit   tau1 = C1.tau;

    size_t i = 0;
    //__________________________________________________________________________
    //
    // search where we enter the function
    //__________________________________________________________________________
    while(i<M)
    {
        if(Tau[i]>tau1) break;
        ++i;
    }
    ans += foot*i;

    //__________________________________________________________________________
    //
    // locate points inside the core
    //__________________________________________________________________________
    const Coord &CN   = coords.back();
    const Unit   tauN = CN.tau;
    size_t       jup  = 2;       // first interval upper index
    while(i<M)
    {
        assert(jup<=N);

        //______________________________________________________________________
        //
        // are we still inside ?
        //______________________________________________________________________
        const Unit tt = Tau[i];
        if(tt>tauN)
            break;

        //______________________________________________________________________
        //
        // check where we are
        //______________________________________________________________________
        assert(tt>coords[jup-1].tau);
        assert(tt<=tauN);
        while(tt>coords[jup].tau)
        {
            assert(jup<N);
            // just slide...
            ++jup;
        }

        //______________________________________________________________________
        //
        // update value
        //______________________________________________________________________
        ans += coords[jup-1].value;
        ++i;
    }

    //__________________________________________________________________________
    //
    // now we leave the function
    //__________________________________________________________________________
    ans += (M-i) * CN.value;

    return ans;
}

