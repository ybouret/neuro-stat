#include "cpw.hpp"

void CPW:: buildFrom(const array<Unit> &train, const Unit delta)
{
    foot = 0;
    free();
    const size_t ns = train.size();
    ensure(2*ns);

    size_t iTrain = 1;
    size_t iShift = 1;

    Unit curr = 0;

    // fusion, iTrain will end first
    while(iTrain<=ns)
    {
        const Unit tauTrain = train[iTrain];
        const Unit tauShift = train[iShift]+delta;
        if(tauTrain<tauShift)
        {
            const coord point(tauTrain,++curr);
            __push_back(point);
            ++iTrain;
        }
        else
        {
            if(tauShift<tauTrain)
            {
                const coord point(tauShift,--curr);
                __push_back(point);
                ++iShift;
            }
            else
            {
                // equality
                ++iShift;
                ++iTrain;
            }
        }
    }

    // final points
    while(iShift<=ns)
    {
        const Unit tauShift = train[iShift]+delta;
        const coord point(tauShift,--curr);
        __push_back(point);
        ++iShift;
    }

}
