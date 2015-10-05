#include "cpw.hpp"

#if 0
Unit CPW:: integrate_(const Unit tauStart, const Unit tauFinal) const
{
    const CPW &self = *this;
    Unit ans=0;
    for(Unit tau=tauStart;tau<=tauFinal;++tau)
    {
        ans += self(tau);
    }
    return ans;
}
#endif


//! integration, naive function
Unit CPW:: integrate(const Unit tauStart, const Unit tauFinal) const
{

    //__________________________________________________________________________
    //
    // Get rid of trivial cases
    //__________________________________________________________________________
    if(tauFinal<=tauStart)
    {
        return 0;
    }

    const size_t N = size;
    switch(N)
    {
        case 0:
            return (tauFinal-tauStart) * foot;

        case 1:
        {
            const coord &C   = (*this)[0];
            const Unit   tau = C.tau;
            if(tauFinal<=tau)
            {
                return (tauFinal-tauStart)*foot;
            }
            else
            {
                const Unit tail = C.value;
                if(tauStart>=tau)
                {
                    return (tauFinal-tauStart)*tail;
                }
                else
                {
                    assert(tauStart<=tau);
                    assert(tau<=tauFinal);
                    return (tau-tauStart) * foot + (tauFinal-tau)*tail;

                }
            }
        }
            // never get here;
            assert(die("never get here"));

        default:
            break;
    }


    return 0;
}
