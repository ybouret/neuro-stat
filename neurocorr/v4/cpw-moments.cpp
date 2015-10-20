#include "cpw.hpp"
void CPW:: evalMoments_(const UArray &tau,
                        const size_t start,
                        const size_t count,
                        Moments     &moments) const throw()
{


    Unit         mu1 = 0;
    Unit         mu2 = 0;

    for(size_t i=start,j=count;j>0;++i,--j)
    {
        const Unit v = (*this)( tau[i] );
        mu1 += v;
        mu2 += v*v;
    }

    moments.mu1 = mu1;
    moments.mu2 = mu2;
}
