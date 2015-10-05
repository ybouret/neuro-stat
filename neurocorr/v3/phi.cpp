#include "phi.hpp"

CPW_Functions :: CPW_Functions(const Train &attached,
                               const size_t extra) :
_CPW_Functions(extra+1),
train(attached)
{
    const size_t K  = capacity; assert(1+extra==K);
    const size_t np = 2*train.size();
    for(size_t k=0;k<K;++k)
    {
        append<size_t>(np);
    }
}

CPW_Functions:: ~CPW_Functions() throw() {}

void CPW_Functions::compute(const Unit delta)
{
    _CPW_Functions &self = *this;
    CPW            &phi  = self[0];
    phi.buildFrom(train,delta);
    for(size_t k=1;k<size;++k)
    {
        const Unit delta_k = k*delta;
        self[k].shiftFrom(phi,delta_k);
    }
}


PHI:: PHI(const Records &records, const size_t extra ) :
_PHI(records.trials,records.neurones),
trials(rows),
neurones(cols),
K(1+extra),
NK(neurones*K),
dim(1+NK),
mixed( (NK*dim)/2 ),
maxCount(records.maxCount),
delta_(0),
run(this, & PHI::computeOn)
{
    procedural_build_with<Records,size_t>(records,extra);
    prepareMixed();
}

PHI:: ~PHI() throw()
{
}

void PHI::compute(const Unit delta, Crew *para)
{
    delta_ = delta;
    if(para)
    {
        //std::cerr << "PHI: Parallel" << std::endl;
        (*para)(run);
    }
    else
    {
        //std::cerr << "PHI: Sequential" << std::endl;
        Crew::single_context mono;
        run(mono);
    }
}

void PHI:: prepareMixed()
{
    //std::cerr << "Preparing " << mixed.capacity << " Mixed terms..." << std::endl;
    for(size_t i=0;i<neurones;++i)
    {
        for(size_t k=0;k<K;++k)
        {
            const size_t I_i_k = i*K + k;
            //std::cerr << "I_i_k=" << I_i_k << ": I_l_m=";
            for(size_t l=i;l<neurones;++l)
            {
                const size_t lk = l*K;
                for(size_t m=0;m<K;++m)
                {
                    const size_t I_l_m = lk+m;
                    if(I_l_m>=I_i_k)
                    {
                        //std::cerr << I_l_m << '/';
                        Mix gm(i,k,l,m,1+I_i_k,1+I_l_m);
                        mixed.push_back(gm);
                    }
                }
            }
            //std::cerr << std::endl;
        }
    }
    //std::cerr << "#mixed=" << mixed.size << "/" << mixed.capacity << std::endl;
}


#include "yocto/code/unroll.hpp"

#define NC_PHI_COMPUTE(II) self(II).compute(delta_)

void PHI:: computeOn(Context &ctx)
{
    _PHI  &self = *this;
    size_t offset = 0;
    size_t length = items;
    ctx.split(offset,length);
#if 0
    {
        YOCTO_LOCK(ctx.access);
        std::cerr << "thread " << ctx.indx << "@" << offset << "+" << length << std::endl;
    }
#endif
    YOCTO_LOOP_FUNC(length,NC_PHI_COMPUTE, offset);
}


