#include "cpw.hpp"


void CPW:: productOf(const CPW &lhs, const CPW &rhs)
{
    assert(this!=&lhs);
    assert(this!=&rhs);

    free();
    const size_t nL = lhs.size;
    const size_t nR = rhs.size;
    const size_t np = nL+nR;
    if(capacity<np)
    {
        resize_empty_to(np);
    }

    Unit L = lhs.foot;
    Unit R = rhs.foot;

    foot = L*R;

    size_t iL = 0;
    size_t iR = 0;

    // mixed area
    while(iL<nL&&iR<nR)
    {
        const coord &lC   = lhs[iL];
        const coord &rC   = rhs[iR];
        const Unit   tauL = lC.tau;
        const Unit   tauR = rC.tau;
        if(tauL<tauR)
        {
            L = lC.value;
            insert(tauL,L*R);
            ++iL;
        }
        else
        {
            if(tauR<tauL)
            {
                R = rC.value;
                insert(tauR,L*R);
                ++iR;
            }
            else
            {
                // equality
                L = lC.value;
                R = rC.value;
                insert(tauR,L*R);
                ++iL;
                ++iR;
            }
        }
    }

    // end of points
    while(iL<nL)
    {
        const coord &C = lhs[iL++];
        insert(C.tau,C.value*R);
    }

    while(iR<nR)
    {
        const coord &C = rhs[iR++];
        insert(C.tau,C.value*L);
    }




}