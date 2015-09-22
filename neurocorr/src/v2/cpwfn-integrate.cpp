#include "cpwfn.hpp"

Real CPW_Function:: integrate( const Unit tauLo, const Unit tauHi) const throw()
{
    assert(tauLo<=tauHi);

    //__________________________________________________________________________
    //
    // Get rid or trivial cases
    //__________________________________________________________________________

    if(tauLo>=tauHi)
    {
        // trivial case 1
        return 0.0;
    }


    const size_t n = coords.size();

    if(n<=0)
    {
        //trivial case 2
        return Real(foot)*Real(tauHi-tauLo);
    }

    const Coord &C1 = coords.front();
    if(tauHi<=C1.tau)
    {
        //trivial case 3
        return Real(foot)*Real(tauHi-tauLo);
    }

    const Coord &Cn = coords.back();
    if(tauLo>=Cn.tau)
    {
        //trivial case 4
        return Real(Cn.value)*Real(tauHi-tauLo);
    }

    //__________________________________________________________________________
    //
    // Less trivial case
    //__________________________________________________________________________
    if(1==n)
    {
        return Real(foot) * Real(C1.tau-tauLo) + Real(C1.value) * (tauHi-C1.tau);
    }

    //__________________________________________________________________________
    //
    // Locate position au tauLo
    //__________________________________________________________________________
    assert(n>=2);
    size_t jlo = 0;
    if(tauLo>C1.tau)
    {
        assert(tauLo<Cn.tau);
        jlo=1;
        size_t jup = n;
        while(jup-jlo>1)
        {
            const size_t jmid   = (jlo+jup)>>1;
            const Unit   tauMid = coords[jmid].tau;
            if(tauLo<tauMid)
            {
                jup = jmid;
            }
            else
            {
                jlo = jmid;
            }
        }
    }

    return 0.0;

}

