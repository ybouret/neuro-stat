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
        assert(n>=2);
        
        
    }
    
}
