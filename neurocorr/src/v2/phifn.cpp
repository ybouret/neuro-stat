#include "phifn.hpp"


PHI_Functions:: ~PHI_Functions() throw()
{
}


PHI_Functions:: PHI_Functions(const size_t extra,
                              const Train &tr) :
CPW_Functions(1+extra),
train(tr)
{
    CPW_Functions &self = *this;

    const Real scale = train.scale;
    for(size_t i=0;i<=extra;++i)
    {
        self.append<Real>(scale);
    }

    const size_t np = train.size() * 2;
    for(size_t i=0;i<size;++i)
    {
        self[i].ensureCapacity(np);
    }

}


void PHI_Functions:: compute(const Unit deltaUnit)
{
    CPW_Functions &self = *this;
    CPW_Function  &phi0 = self[0];
    phi0.buildFrom(train,deltaUnit);
    const Unit top(size);
    for(Unit i=1;i<top;++i)
    {
        self[i].copyAndShift(phi0,i*deltaUnit);
    }
}


PHI:: ~PHI() throw()
{
}

PHI:: PHI(const size_t extra, const Records &records) :
PHI_Base(records.rows,records.cols),
trials(rows),
neurones(cols),
K(1+extra),
NK(neurones*K),
dim(1+neurones*K),
mixed( (NK*dim)/2 ),
maxCount(records.maxCount),
delta(0),
pCode(this,& PHI::paraCompute)
{
    build_with<PHI_Functions *>(NULL);
    PHI_Base &self = *this;
    for(size_t i=0;i<trials;++i)
    {
        for(size_t j=0;j<neurones;++j)
        {
            self[i][j].reset( new PHI_Functions(extra, * records[i][j] ) );
        }
    }
    prepareMixed();
}

#include "yocto/exception.hpp"
#include "yocto/code/unroll.hpp"
#define NC_COMPUTE_PHI(I) self(I)->compute(delta)

void PHI:: compute( const Unit deltaUnit, Crew *para )
{
    delta = deltaUnit;
    if(para)
    {
        (*para)(pCode);
        if(para->failure)
        {
            throw exception("something went wrong in parallel compute");
        }
    }
    else
    {
        PHI_Base &self = *this;
        YOCTO_LOOP_FUNC(items, NC_COMPUTE_PHI,0);
    }

}



void PHI:: paraCompute(Context &ctx)
{
    PHI_Base &self   = *this;
    size_t    offset = 0;
    size_t    length = items;
    ctx.split(offset, length);
    YOCTO_LOOP_FUNC(length, NC_COMPUTE_PHI,offset);
}

void PHI:: prepareMixed()
{
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
    std::cerr << "#mixed=" << mixed.size << "/" << mixed.capacity << std::endl;
}

