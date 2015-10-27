#include "cpw.hpp"

Unit CPW:: integrate_(const Unit tauStart, const Unit tauFinal) const throw()
{
    if(tauStart>=tauFinal)
    {
        return 0;
    }
    else
    {
        const CPW &self = *this;
        Unit ans = 0;
        for(Unit tau = tauStart; tau < tauFinal; ++tau )
        {
            ans += self(tau+1);
        }
        return ans;
    }
}

Unit CPW:: integrate(Unit tauStart, Unit tauFinal) const throw()
{

    if(tauStart>=tauFinal)
    {
        return 0;
    }
    else
    {
        const size_t n = size();
        switch(n)
        {
            case 0:
                //______________________________________________________________
                //
                // a constant function
                //______________________________________________________________
                return (tauFinal-tauStart) * foot;


            case 1:
                //______________________________________________________________
                //
                // one point only
                //______________________________________________________________
            {
                const coord cc = front();
                const Unit  tt = cc.tau;
                if(tauFinal<=tt)
                {
                    // everybody at left
                    return (tauFinal-tauStart) * foot;
                }
                else
                {
                    if(tauStart>=tt)
                    {
                        // everybody at right
                        return (tauFinal-tauStart) * cc.value;
                    }
                    else
                    {
                        // somewhere in the middle
                        return (tt-tauStart) * foot + (tauFinal-tt) * cc.value;
                    }
                }

            }
                assert(die("never get here"));

            default:
                break;

        }
        assert(n>=2);
        //______________________________________________________________________
        //
        // Generic case
        //______________________________________________________________________
        const coord cmin = front();
        if(tauFinal<=cmin.tau)
        {
            //every body is at left
            //std::cerr << "-" << std::endl;
            return (tauFinal-tauStart) * foot;
        }
        else
        {
            const coord cmax = back();
            if(tauStart>=cmax.tau)
            {
                //std::cerr << "+" << std::endl;
                return (tauFinal-tauStart) * cmax.value;
            }
            else
            {

                const CPW &self = *this;
                Unit       ans  = 0;

                //______________________________________________________________
                //
                // most generic case
                //______________________________________________________________

                //--------------------------------------------------------------
                // study the left side
                //--------------------------------------------------------------
                size_t jmin_lo = 1;
                size_t jmin_up = n;
                if(tauStart<cmin.tau)
                {
                    std::cerr << "a";
                    ans     += (cmin.tau-tauStart) * foot;
                    tauStart = cmin.tau;
                    jmin_up  = 2;
                    assert(self[jmin_lo].tau<=tauStart);
                    assert(tauStart<self[jmin_up].tau);
                }
                else
                {
                    std::cerr << "b";
                    //locate self[jmin_lo].tau <= tauStart < self[jmin_up].tau
                    assert(tauStart<cmax.tau);
                    while(jmin_up-jmin_lo>1)
                    {
                        const size_t jmid = (jmin_lo+jmin_up)>>1;
                        const Unit   tmid = self[jmid].tau;
                        if(tmid<=tauStart)
                        {
                            jmin_lo = jmid;
                        }
                        else
                        {
                            jmin_up = jmid;
                        }
                    }
                    assert(self[jmin_lo].tau<=tauStart);
                    assert(tauStart<self[jmin_up].tau);
                }

                //--------------------------------------------------------------
                // study the right side
                //--------------------------------------------------------------
                size_t jmax_lo = jmin_lo; assert(tauFinal>=self[jmax_lo].tau);
                size_t jmax_up = n;
                if(tauFinal>=cmax.tau)
                {
                    std::cerr << "c";
                    ans      += (tauFinal-cmax.tau) * cmax.value;
                    jmax_lo   = n-1;
                    tauFinal  = cmax.tau;
                    assert(self[jmax_lo].tau<=tauFinal);
                    assert(tauFinal<=self[jmax_up].tau);
                }
                else
                {
                    std::cerr << "d";
                    assert(tauFinal<cmax.tau);
                    //locate self[jmax_lo].tau <= tauFinal < self[jmax_up].tau
                    while(jmax_up-jmax_lo>1)
                    {
                        const size_t jmid = (jmax_lo+jmax_up)>>1;
                        const Unit   tmid = self[jmid].tau;
                        if(tmid<=tauStart)
                        {
                            jmax_lo = jmid;
                        }
                        else
                        {
                            jmax_up = jmid;
                        }
                    }
                    assert(self[jmax_lo].tau<=tauFinal);
                    assert(tauFinal<self[jmax_up].tau);
                }

                //______________________________________________________________
                //
                // Ok, now times are set and bracketed
                //______________________________________________________________
                assert(jmax_lo>=jmin_lo);
                if(jmax_lo<=jmin_lo)
                {
                    //----------------------------------------------------------
                    // in the same interval
                    //----------------------------------------------------------
                    std::cerr << "e";
                    ans += (tauFinal-tauStart) * self[jmin_lo].value;
                }
                else
                {
                    //----------------------------------------------------------
                    // different interval
                    //----------------------------------------------------------
                    std::cerr << "f";
                }

                return ans;
            }
        }
    }
}