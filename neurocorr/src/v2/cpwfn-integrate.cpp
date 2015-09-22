#include "cpwfn.hpp"

Real CPW_Function:: integrate(const Unit tauLo,
                              const Unit tauHi) const throw()
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
    // Less trivial case: 1 point across the interval
    //__________________________________________________________________________
    if(1==n)
    {
        return Real(foot) * Real(C1.tau-tauLo) + Real(C1.value) * (tauHi-C1.tau);
    }

    //__________________________________________________________________________
    //
    // Locate position for tauLo
    //__________________________________________________________________________
    Real ans  = 0.0;
    assert(n>=2);
    size_t jlo = 0;
    //std::cerr << "tauLo=" << tauLo << ", tau[1]=" << C1.tau << std::endl;
    if(tauLo>=C1.tau)
    {
        assert(tauLo<Cn.tau);
        jlo=1;
        size_t jup = 1;
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
        std::cerr << "lo:" << coords[jlo].tau << "<=" << tauLo << "<=" << coords[jup].tau << std::endl;
    }

    //__________________________________________________________________________
    //
    // Locate position for tauHi
    //__________________________________________________________________________
    size_t jhi = n;
    if(tauHi<=Cn.tau)
    {
        assert(tauHi>C1.tau);
        jhi = (jlo>0) ? jlo : 1;
        size_t jbn = n;
        while(jbn-jhi>1)
        {
            const size_t jmid   = (jhi+jbn)>>1;
            const Unit   tauMid = coords[jmid].tau;
            if(tauHi<tauMid)
            {
                jbn = jmid;
            }
            else
            {
                jhi = jmid;
            }
        }
        std::cerr << "hi:" << coords[jhi].tau << "<=" << tauHi << "<=" << coords[jbn].tau << std::endl;
    }
    
    
    return ans;

}

