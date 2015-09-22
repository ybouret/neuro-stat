#include "cpwfn.hpp"

void CPW_Function:: buildFrom(const RArray<Unit> &train,
                              const Unit          deltaUnit,
                              const bool          cleanUp)
{
    assert(deltaUnit>0);
    const size_t n = train.size();
    coords.free();
    coords.ensure(2*n);

    CVector<Unit> shift(n);
    for(size_t i=0;i<n;++i)
    {
        shift[i] = train[i] + deltaUnit;
    }

    size_t iShift = 0;
    size_t iTrain = 0;
    Unit   curr   = 0;
    foot          = 0;
    while(iTrain<n)
    {
        const Unit tt = train[iTrain];
        const Unit ts = shift[iShift];
        if(tt<=ts)
        {
            ++curr;
            const Coord C(tt,curr);
            coords.push_back(C);
            ++iTrain;
        }
        else
        {
            --curr;
            const Coord C(ts,curr);
            coords.push_back(C);
            ++iShift;
        }
    }

    while(iShift<n)
    {
        --curr;
        const Coord C(shift[iShift++],curr);
        coords.push_back(C);
    }

    assert(2*n==coords.size());

    if(cleanUp) removeEmptyIntervals();
}

void CPW_Function:: removeEmptyIntervals() throw()
{
    // the first and last point cannot be multiple
    size_t i = size();
    if(i>3)
    {
        --i;
        size_t im = i-1;
        while(im>1)
        {
            Coord &Cm = coords[im];
            Coord &C  = coords[i];
            if(Cm.tau>=C.tau)
            {
                std::cerr << "Should remove tau=" << Cm.tau << std::endl;
                const Coord &Cp = coords[i+1];
                memmove(&Cm, &Cp, sizeof(Coord) * (coords.size()-i) );
                coords.pop_back();
                coords.pop_back();
            }
            --im;
            --i;
        }
    }
}
