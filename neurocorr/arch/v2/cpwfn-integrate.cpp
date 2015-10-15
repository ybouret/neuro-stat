#include "cpwfn.hpp"


namespace
{
    static inline
    size_t FindLowerIndexOf(const Unit          tau,
                            const array<Coord> &coords,
                            size_t              jlo,
                            size_t              jup ) throw()
    {
        assert(coords.size()>=2);
        assert(tau>=coords[jlo].tau);
        assert(tau<=coords[jup].tau);
        assert(jup>jlo);
        while(jup-jlo>1)
        {
            const size_t jmid = (jup+jlo)>>1;
            const Unit   tmid = coords[jmid].tau;
            if(tau<tmid)
            {
                jup = jmid;
            }
            else
            {
                jlo = jmid;
            }
        }
        return jlo;
    }
}

Unit CPW_Function:: integrate(Unit tauLo,
                              Unit tauHi) const throw()
{
    assert(tauLo<=tauHi);

    //__________________________________________________________________________
    //
    // Get rid or most trivial case
    //__________________________________________________________________________
    if(tauLo>=tauHi)
    {
        // trivial case 1
        return 0.0;
    }


    //__________________________________________________________________________
    //
    // Get rid or  trivial cases
    //__________________________________________________________________________
    const size_t N = coords.size();

    if(N<=0)
    {
        //trivial case 2: not points, constant function
        return foot * (tauHi-tauLo);
    }

    const Coord &C1   = coords.front();
    const Unit   tau1 = C1.tau;
    if(tauHi<=tau1)
    {
        //trivial case 3: integral at left of interval
        return foot*(tauHi-tauLo);
    }

    const Coord &CN   = coords.back();
    const Unit   tauN = CN.tau;
    if(tauLo>=tauN)
    {
        //trivial case 4
        return CN.value*(tauHi-tauLo);
    }

    //__________________________________________________________________________
    //
    // Less trivial case: 1 point across the interval
    //__________________________________________________________________________
    if(1==N)
    {
        return foot*(tau1-tauLo) + C1.value * (tauHi-tau1);
    }

    //__________________________________________________________________________
    //
    // Locate position for tauLo
    //__________________________________________________________________________
    assert(N>=2);

    Unit   ans = 0;
    size_t jlo = 1;
    if(tauLo<=tau1)
    {
        // compute foot value and forward tauLo
        ans   = foot * (tau1-tauLo);
        tauLo = tau1;
    }
    else
    {
        assert(tauLo<tauN);
        // full range bissection
        jlo = FindLowerIndexOf(tauLo, coords, 1, N);
    }

    //__________________________________________________________________________
    //
    // Locate position for tauHi
    //__________________________________________________________________________
    size_t jhi = N-1;
    if(tauHi>=tauN)
    {
        // compute tail value and move tauLo
        ans  += CN.value * (tauHi-tauN);
        tauHi = tauN;
    }
    else
    {
        assert(tauHi>tau1);
        // bissection using jlo as starting point
        jhi = FindLowerIndexOf(tauHi, coords, jlo, N);
    }


    //__________________________________________________________________________
    //
    // Final Sum
    //__________________________________________________________________________
    if(jhi<=jlo)
    {
        // they are in the same interval !
        ans += coords[jlo].value * (tauHi-tauLo);
    }
    else
    {
        // interval containing tauLo
        const size_t jup = jlo + 1;
        ans += coords[jlo].value * (coords[jup].tau-tauLo);

        // core
        for(size_t j=jup,jp=jup+1;j<jhi;++j,++jp)
        {
            const Coord &C = coords[j];
            ans += C.value * (coords[jp].tau - C.tau);
        }

        // interval containing tauHi
        const Coord &hi = coords[jhi];
        ans += hi.value * (tauHi - hi.tau);

    }

    return ans;
    
}

