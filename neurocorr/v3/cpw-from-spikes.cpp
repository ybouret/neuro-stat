#include "cpw.hpp"
#include "yocto/code/unroll.hpp"

#define NC_ADD_SHIFT(II) shift[II] = train[II]+delta



void CPW:: buildFrom(const RArray<Unit> &train, const Unit delta)
{
    assert(delta>0);
    free();
    foot=0;
    const size_t np = train.size();
    const size_t maxPoints = np*2;
    if(capacity<maxPoints)
    {
        resize_empty_to(maxPoints);
    }

    //create a temporary, shifted array
    CVector<Unit> shift(np);
    YOCTO_LOOP_FUNC(np,NC_ADD_SHIFT,0);

    //optimized fusion algorithm
    size_t iTrain = 0;
    size_t iShift = 0;
    Unit   curr   = 0;

    // iTrain will always finish before iShift
    while(iTrain<np)
    {
        assert(iShift<np);
        const Unit tr = train[iTrain];
        const Unit sh = shift[iShift];
        if(tr<sh)
        {
            insert(tr,++curr);
            ++iTrain;
        }
        else
        {
            if(sh<tr)
            {
                insert(sh,--curr);
                ++iShift;
            }
            else
            {
                // cancellation of each term
                ++iTrain;
                ++iShift;
            }
        }
    }

    // finalize
    while(iShift<np)
    {
        insert(shift[iShift++],--curr);
    }

}
