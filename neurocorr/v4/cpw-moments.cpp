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

#include "yocto/sequence/lw-array.hpp"

void CPW:: evalMoments(const UArray &tau_,
                       const size_t start,
                       const size_t count,
                       Moments     &moments) const throw()
{

    Unit         mu1 = 0;
    Unit         mu2 = 0;
    if(count<=0)
        goto FINALIZE;
    else
    {
        //______________________________________________________________________
        //
        // a least one point to evaluate
        //______________________________________________________________________
        assert(count>0);
        assert(start>=1);
        assert(start+count-1<=tau_.size());
        const size_t         n = this->size();
        const lw_array<Unit> tau( (Unit *)&tau_[start], count);
        switch (n)
        {
                //______________________________________________________________
                //
                // constant function
                //______________________________________________________________
            case 0: mu1 = count * foot; mu2 = mu1 * foot; goto FINALIZE;


                //______________________________________________________________
                //
                // one splitting point
                //______________________________________________________________
            case 1:
            {
                const coord C     = front(); // the one coordinate
                const Unit  tt    = C.tau;   // the splitting value
                const Unit  lower = tau[1];
                if(tt<lower)
                {
                    // all tau's at right
                    mu1 = count * C.value; mu2 = mu1 * C.value; goto FINALIZE;
                }
                else
                {
                    const Unit upper = tau[count];
                    if(tt>=upper)
                    {
                        // all tau's at left
                        mu1 = count * foot; mu2 = mu1 * foot; goto FINALIZE;
                    }
                    else
                    {
                        assert(tt>=lower);
                        assert(tt<upper);
                        assert(count>1);
                        size_t jlo = 1;
                        size_t jup = count;
                        while(jup-jlo>1)
                        {
                            const size_t jmid = (jlo+jup)>>1;
                            const Unit   tmid = tau[jmid];
                            if(tt<tmid)
                            {
                                jup = jmid;
                            }
                            else
                            {
                                jlo = jmid;
                            }
                        }
                        assert(1==jup-jlo);
                        assert(tt<tau[jup]);
                        assert(tau[jlo]<=tt);

                        const Unit nlo = jlo;
                        const Unit vlo = foot;
                        const Unit nhi = (count-jlo);
                        const Unit vhi = C.value;

                        const Unit plo = nlo * vlo;
                        const Unit phi = nhi * vhi;
                        mu1 = plo     + phi;
                        mu2 = plo*vlo + phi*vhi;

                        goto FINALIZE;
                    }
                }
            }


            default:
                break;
        }
        assert(n>=2);
        assert(count>0);

        const coord bot   = front();
        const Unit  upper = tau[count];
        if(upper<=bot.tau)
        {
            //everybody at left
            mu1 = count * foot; mu2 = mu1 * foot; goto FINALIZE;
        }
        else
        {
            const coord top   = back();
            const Unit  lower = tau[1];
            if(lower>=top.tau)
            {
                //everybody at right
                mu1 = count * top.value; mu2 = mu1 * top.value; goto FINALIZE;
            }
            else
            {
                //______________________________________________________________
                //
                //
                // Generic case, at least one point
                //
                //______________________________________________________________
                size_t i=1;
                for(;i<=count;++i)
                {
                    if(bot.tau<tau[i])
                    {
                        break;
                    }
                }

                //______________________________________________________________
                //
                // initialize with the number of points before bot.tau
                //______________________________________________________________
                {
                    const Unit nf = i-1;
                    mu1 = nf  * foot;
                    mu2 = mu1 * foot;
                }

                //______________________________________________________________
                //
                // locate self[jlo].tau<tau[i]<=self[jup].tau
                //______________________________________________________________
                const _CPW &self = *this;
                size_t      jlo  = 1;
                size_t      jup  = n;
                {
                    const Unit curr = tau[i]; assert(bot.tau<curr); assert(curr<=top.tau);
                    while(jup-jlo>1)
                    {
                        const size_t jmid = (jlo+jup)>>1;
                        const Unit   tmid = self[jmid].tau;
                        if(tmid<curr)
                        {
                            jlo = jmid;
                        }
                        else
                        {
                            jup = jmid;
                        }
                    }
                    assert(1==jup-jlo);
                    assert(self[jlo].tau<tau[i]);
                    assert(tau[i]<=self[jup].tau);
                }

                // start with a point between jlo and jup
                for(;i<=count;++i)
                {
                    const Unit curr = tau[i];
                    if(curr>top.tau)
                    {
                        break;
                    }
                    else
                    {
                        while( !( (self[jlo].tau<curr) && (curr<=self[jup].tau) ) )
                        {
                            ++jlo;
                            ++jup;
                        }
                        const Unit value = self[jlo].value;
                        mu1 += value;
                        mu2 += value*value;
                    }
                }

                //______________________________________________________________
                //
                // finalize with the number of points after top.tau
                //______________________________________________________________
                {
                    const Unit remaining = (count+1-i);
                    const Unit tmp       = remaining*top.value;
                    mu1 += tmp;
                    mu2 += tmp * top.value;
                }
            }
        }
        
    }
    
FINALIZE:
    moments.mu1 = mu1;
    moments.mu2 = mu2;
}
