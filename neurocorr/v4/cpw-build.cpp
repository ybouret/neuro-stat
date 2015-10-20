#include "cpw.hpp"

void CPW:: buildFrom(const UArray &train, const Unit delta, UVector &shift)
{
    foot = 0;
    
    free();
    shift.free();
    
    const size_t ns = train.size();
    ensure(2*ns);     assert(this->capacity()>=2*ns);
    shift.ensure(ns); assert(shift.capacity()>=ns);
    
    for(size_t i=1;i<=ns;++i)
    {
        shift.__push_back( train[i]+delta );
    }
    assert(ns==shift.size());
    size_t iTrain = 1;
    size_t iShift = 1;

    Unit curr = 0;
    
    
    // fusion, iTrain will end first
    while(iTrain<=ns)
    {
        const Unit tauTrain = train[iTrain];
        const Unit tauShift = shift[iShift];
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
        const coord point(shift[iShift],--curr);
        __push_back(point);
        ++iShift;
    }

}
