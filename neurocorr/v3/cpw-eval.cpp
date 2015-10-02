#include "cpw.hpp"

Unit CPW:: operator()(const Unit tau) const throw()
{
    const size_t np   =  size;
    const _CPW  &self = *this;
    switch (np) {
        case 0:
            return foot;

        case 1:
        {
            const coord &C = self[0];
            return (tau <= C.tau) ? foot  : C.value;
        }

        default:
            break;
    }

    assert(np>=2);
    //const size_t top = np-1;
    const coord &Bot = self[0];
    if(tau<=Bot.tau)
    {
        return foot;
    }
    else
    {
        const size_t  top = np-1;
        const coord  &Top = self[top];
        if(tau>Top.tau)
        {
            return Top.value;
        }
        else
        {
            // coordinate is somewhere inside
            size_t jlo = 0;
            size_t jup = top;
            while(jup-jlo>1)
            {
                const size_t jmid = (jlo+jup)>>1;
                const coord &cmid = self[jmid];
                const Unit   tmid = cmid.tau;
                if(tau<=tmid)
                {
                    jup = jmid;
                }
                else
                {
                    jlo = jmid;
                }
            }
            return self[jlo].value;
        }
    }

}
