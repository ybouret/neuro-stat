#include "cpw.hpp"

void CPW::product(const CPW &lhs, const CPW &rhs)
{
    free();
    const size_t nL = lhs.size();
    const size_t nR = rhs.size();
    const size_t np = nL+nR;
    ensure(np);

    Unit L = lhs.foot;
    Unit R = rhs.foot;

    foot = L*R;

    size_t iL = 1;
    size_t iR = 1;

    // mixed area
    while(iL<=nL&&iR<=nR)
    {
        const coord &lC   = lhs[iL];
        const coord &rC   = rhs[iR];
        const Unit   tauL = lC.tau;
        const Unit   tauR = rC.tau;
        if(tauL<tauR)
        {
            L = lC.value;
            __add(tauL,L*R);
            ++iL;
        }
        else
        {
            if(tauR<tauL)
            {
                R = rC.value;
                __add(tauR,L*R);
                ++iR;
            }
            else
            {
                // equality
                L = lC.value;
                R = rC.value;
                __add(tauR,L*R);
                ++iL;
                ++iR;
            }
        }
    }

    // end of points, one of the data is done
    while(iL<=nL)
    {
        const coord &C = lhs[iL++];
        __add(C.tau,C.value*R);
    }

    while(iR<=nR)
    {
        const coord &C = rhs[iR++];
        __add(C.tau,C.value*L);
    }
    

}