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
                size_t jstart_lo = 1;
                size_t jstart_up = n;
                if(tauStart<cmin.tau)
                {
                    ans     += (cmin.tau-tauStart) * foot;
                    tauStart = cmin.tau;
                    jstart_up  = 2;
                    assert(self[jstart_lo].tau<=tauStart);
                    assert(tauStart<self[jstart_up].tau);
                }
                else
                {
                    //----------------------------------------------------------
                    // self[jstart_lo].tau <= tauStart < self[jstart_up].tau
                    //----------------------------------------------------------
                    assert(tauStart<cmax.tau);
                    while(jstart_up-jstart_lo>1)
                    {
                        const size_t jmid = (jstart_lo+jstart_up)>>1;
                        const Unit   tmid = self[jmid].tau;
                        if(tmid<=tauStart)
                        {
                            jstart_lo = jmid;
                        }
                        else
                        {
                            jstart_up = jmid;
                        }
                    }
                    assert(self[jstart_lo].tau<=tauStart);
                    assert(tauStart<self[jstart_up].tau);
                }

                //--------------------------------------------------------------
                // study the right side
                //--------------------------------------------------------------
                size_t jfinal_lo = jstart_lo; assert(tauFinal>=self[jfinal_lo].tau);
                size_t jfinal_up = n;
                if(tauFinal>=cmax.tau)
                {
                    ans      += (tauFinal-cmax.tau) * cmax.value;
                    jfinal_lo   = n-1;
                    tauFinal  = cmax.tau;
                    assert(self[jfinal_lo].tau<=tauFinal);
                    assert(tauFinal<=self[jfinal_up].tau);
                }
                else
                {
                    assert(tauFinal<cmax.tau);
                    //----------------------------------------------------------
                    // self[jfinal_lo].tau <= tauFinal < self[jfinal_up].tau
                    //----------------------------------------------------------
                    while(jfinal_up-jfinal_lo>1)
                    {
                        const size_t jmid = (jfinal_lo+jfinal_up)>>1;
                        const Unit   tmid = self[jmid].tau;
                        if(tmid<=tauFinal)
                        {
                            jfinal_lo = jmid;
                        }
                        else
                        {
                            jfinal_up = jmid;
                        }
                    }
                    assert(self[jfinal_lo].tau<=tauFinal);
                    assert(tauFinal<self[jfinal_up].tau);
                }

                //______________________________________________________________
                //
                // Ok, now times are set and bracketed
                //______________________________________________________________
                assert(jfinal_lo>=jstart_lo);
                if(jfinal_lo<=jstart_lo)
                {
                    //----------------------------------------------------------
                    // in the same interval
                    //----------------------------------------------------------
                    ans += (tauFinal-tauStart) * self[jstart_lo].value;
                }
                else
                {
                    //----------------------------------------------------------
                    // different intervals
                    //----------------------------------------------------------

                    //----------------------------------------------------------
                    // value from interval containing tauStart
                    //----------------------------------------------------------
                    ans += (self[jstart_up].tau - tauStart) * self[jstart_lo].value;


                    //----------------------------------------------------------
                    // values in the middle
                    //----------------------------------------------------------
                    for(size_t j=jstart_up,jp=j+1;j<jfinal_lo;++j,++jp)
                    {
                        const coord c( self[j] );
                        ans += (self[jp].tau - c.tau) * c.value;
                    }

                    //----------------------------------------------------------
                    // value from interfval containing tauFinal
                    //----------------------------------------------------------
                    ans += (tauFinal - self[jfinal_lo].tau) * self[jfinal_lo].value;

                }

                return ans;
            }
        }
    }
}
