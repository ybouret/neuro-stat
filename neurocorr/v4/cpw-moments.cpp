#include "cpw.hpp"
void CPW:: evalMoments_(const UArray &tau,
                        const size_t start,
                        const size_t count,
                        Moments     &moments) const throw()
{


    const CPW   &self = *this;
    Unit         mu1 = 0;
    Unit         mu2 = 0;

    for(size_t i=start,j=count;j>0;++i,--j)
    {
        const Unit v = self( tau[i] );
        mu1 += v;
        mu2 += v*v;
    }

    moments.mu1 = mu1;
    moments.mu2 = mu2;
}

#include "yocto/sequence/lw-array.hpp"
#include "yocto/code/bzset.hpp"


void CPW:: evalMoments(const UArray &tau_,
                       const size_t start,
                       const size_t count,
                       Moments     &moments) const throw()
{

    //__________________________________________________________________________
    //
    // initializing
    //__________________________________________________________________________
    Unit &mu1 = moments.mu1;
    Unit &mu2 = moments.mu2;
    mu1=mu2=0;

    if(count>0)
    {
        //______________________________________________________________________
        //
        // We have at least one tau to eval: how many CPW points do we have ?
        //______________________________________________________________________
        const lw_array<Unit> tau( (Unit *)&tau_[start], count );
        const size_t         n = this->size();

        switch(n)
        {

            case 0:
                //______________________________________________________________
                //
                // most trivial case
                //______________________________________________________________
                mu1 = count*foot;
                mu2 = mu1*foot;
                return;

            case 1:
                //______________________________________________________________
                //
                // one point in the partition: where is it within
                // the tau ?
                //______________________________________________________________
            {
                const coord C  = front();
                const Unit  tt = C.tau;

                const Unit tauMin = tau[1];
                if( tt < tauMin )
                {
                    // all the points at right => C.value
                    mu1 = count * C.value;
                    mu2 = mu1   * C.value;
                    return;
                }
                else
                {
                    const Unit tauMax = tau[count];
                    if(tt>=tauMax)
                    {
                        // all the points at left => C.value
                        mu1 = count*foot;
                        mu2 = mu1*foot;
                        return;
                    }
                    else
                    {
                        assert(tt>=tauMin);
                        assert(tt<tauMax);
                        assert(count>=2);
                        // need to locate tau[jlo] <= tt < tau[jup]
                        size_t jlo = 1;
                        size_t jup = count;
                        while(jup-jlo>1)
                        {
                            const size_t jmid = (jup+jlo)>>1;
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
                        assert(jup-jlo==1);
                        assert(tau[jlo]<=tt);
                        assert(tt<tau[jup]);
                        //std::cerr << "+" << std::endl;
                        const Unit nf = jlo;
                        const Unit nt = count-jlo;

                        const Unit vf = foot;
                        const Unit vt = C.value;

                        const Unit nvf = nf * vf;
                        const Unit nvt = nt * vt;

                        mu1 = nvf + nvt;
                        mu2 = nvf * vf + nvt * vt;
                        return;
                    }
                }
            }
                assert(die("never get here"));

            default: break;
        }

        //______________________________________________________________________
        //
        //
        // Generic case we have multiple partition points
        // and a few tau's
        //
        //______________________________________________________________________
        assert(n>=2);

        //
        // trivial cases
        //______________________________________________________________________
        const coord cmin   = front();
        const Unit  tauMax = tau[count];
        if(tauMax<=cmin.tau)
        {
            //__________________________________________________________________
            //
            // everybody at left
            //__________________________________________________________________
            mu1 = count * foot;
            mu2 = mu1   * foot;
            return;
        }
        else
        {
            const coord cmax   = back();
            const Unit  tauMin = tau[1];
            if(tauMin>cmax.tau)
            {
                //______________________________________________________________
                //
                // everybody at right
                //______________________________________________________________
                mu1 = count * cmax.value;
                mu2 = mu1   * cmax.value;
                return;
            }
            else
            {
                //______________________________________________________________
                //
                // Most generic case: at least one point within the partition
                //______________________________________________________________
                assert(tauMin<=cmax.tau);
                assert(tauMax>cmin.tau);

                //______________________________________________________________
                //
                // Let us find how tau's are <= cmin.tau
                //______________________________________________________________
                size_t      i    = 1;
                for(;i<count;++i)
                {
                    if(tau[i]>cmin.tau)
                    {
                        break;
                    }
                }

                //______________________________________________________________
                //
                // initialize if any
                //______________________________________________________________
                if(i>1)
                {
                    const Unit nf = i-1;
                    const Unit vf = foot;
                    mu1 = nf  * vf;
                    mu2 = mu1 * vf;
                }

                assert(cmin.tau<tau[i]);
                if(tau[i]<=cmax.tau)
                {

                    //______________________________________________________________
                    //
                    // locate  first point: self[jlo].tau < tau[i] <= self[jhi].tau
                    //______________________________________________________________
                    const CPW &self = *this;
                    size_t jlo = 1;
                    size_t jup = n;
                    {
                        const Unit curr = tau[i];
                        while(jup-jlo>1)
                        {
                            const size_t jmid = (jup+jlo)>>1;
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
                    }
                    assert(1==jup-jlo);
                    assert(self[jlo].tau<tau[i]);
                    assert(tau[i]<=self[jup].tau);

                    for(;i<=count;++i)
                    {
                        const Unit curr = tau[i];
                        if(curr>cmax.tau)
                            break;
                        assert(self[jlo].tau<tau[i]);
                        if(curr>self[jup].tau)
                        {
                            jup = n; assert(jup-jlo>1);
                            while(jup-jlo>1)
                            {
                                const size_t jmid = (jup+jlo)>>1;
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

                        }
                        assert(1==jup-jlo);
                        assert(self[jlo].tau<tau[i]);
                        assert(tau[i]<=self[jup].tau);

                        const Unit value = self[jlo].value;
                        mu1 += value;
                        mu2 += value*value;
                    }
                }
                
                const size_t remaining = count-(i-1);
                //std::cerr << "remaining=" << remaining << std::endl;
                if(remaining)
                {
                    assert(i<=count);
                    assert(tau[i]>cmax.tau);
                    mu1 += cmax.value;
                    mu2 += cmax.value*cmax.value;
                }
                
                return;
            }
        }
        assert(die("Never Get Here!"));
        
        
        
    }
    
}
