#include "train.hpp"


size_t Train:: locateIndicesWithin_(const Unit tauStart,
                                    const Unit tauFinal,
                                    size_t    &starting ) const throw()
{
    if(tauStart>=tauFinal)
        return 0;
    else
    {
        const size_t  n    = size();
        const _Train &self = *this;
        size_t i=1;
        size_t org=0;
        for(;i<=n;++i)
        {
            if(self[i]>tauStart)
            {
                org = i;
                break;
            }
        }

        if(org<=0)
        {
            return 0;
        }

        for(;i<=n;++i)
        {
            if(self[i]>tauFinal)
            {
                break;
            }
        }

        starting = org;
        return i-org;
    }
}


//! find indices such that tauStart<train[i]<=tauFinal
size_t Train:: locateIndicesWithin( const Unit tauStart, const Unit tauFinal, size_t &starting ) const throw()
{
    if(tauStart>=tauFinal)
        return 0;
    else
    {
        const size_t  n    = size();
        //std::cerr << "n=" << n << std::endl;
        //______________________________________________________________________
        //
        // Get rid of trivial cases, level 1
        //______________________________________________________________________
        switch(n)
        {
            case 0:
                return 0;

            case 1:
            {
                const Unit tau = front();
                if(tauStart<tau&&tau<=tauFinal)
                {
                    starting=1;
                    return 1;
                }
                else
                {
                    return 0;
                }
            }

            default:
                break;
        }

        assert(n>=2);

        //______________________________________________________________________
        //
        // Get rid of trivial cases, level 2
        //______________________________________________________________________
        const _Train &tau   = *this;
        const Unit    lower = tau.front();
        if(tauFinal<lower)
        {
            // everybody at left
            return 0;
        }
        else
        {
            const Unit upper = tau.back();
            if(tauStart>=upper)
            {
                //everybody at right
                return 0;
            }
            else
            {

                //______________________________________________________________
                //
                // generic case
                //______________________________________________________________

                size_t org = 1; //! valid if tauStart < lower
                size_t jlo = 1;
                if(tauStart>=lower)
                {
                    // find jup such that tau[jup]>tauStart
                    size_t &jup = org;
                    jup = n;
                    while(jup-jlo>1)
                    {
                        const size_t jmid = (jup+jlo)>>1;
                        const Unit   tmid = tau[jmid];
                        if(tauStart<tmid)
                        {
                            jup = jmid;
                        }
                        else
                        {
                            jlo = jmid;
                        }
                    }
                    assert(1==jup-jlo);
                    assert(tauStart<tau[jup]);
                    assert(tau[jlo]<=tauStart);
                }
                assert(tauStart<tau[org]);

                size_t end = n; //! valid if tauFinal>=upper
                if(tauFinal<upper)
                {
                    // find end such that tauFinal>=tau[end]
                    size_t jup = n;
                    while(jup-jlo>1)
                    {
                        const size_t jmid = (jup+jlo)>>1;
                        const Unit   tmid = tau[jmid];
                        if(tmid<=tauFinal)
                        {
                            jlo = jmid;
                        }
                        else
                        {
                            jup = jmid;
                        }
                    }
                    assert(1==jup-jlo);
                    assert(tau[jlo]<=tauFinal);
                    assert(tau[jup]>tauFinal);
                    end = jlo;
                }
                assert(tau[end]<=tauFinal);
                starting = org;
                return ++end-org;
            }
        }

    }

}
