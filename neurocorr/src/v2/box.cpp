#include "box.hpp"


Box:: ~Box() throw()
{
}

Box:: Box(const size_t indx,
          const Unit   ini,
          const Unit   end) throw() :
trial(indx),
tauStart(ini),
tauFinal(end),
kind(0)
{
    assert(tauFinal>=tauStart);
}


Box:: Box(const Box &b) throw() :
trial(b.trial),
tauStart(b.tauStart),
tauFinal(b.tauFinal),
kind(b.kind)
{

}


void Box:: extract( UList &Tau, const Train &train ) const
{
    Tau.free();
    const size_t ns = train.size();
    size_t i=0;

    while(i<ns)
    {
        if(train[i]>=tauStart) break;
        ++i;
    }

    while(i<ns)
    {
        const Unit tt = train[i];
        if(tt>tauFinal) break;
        Tau.push_back(tt);
        ++i;
    }

}


void Box:: appendRHS(const PHI &Phi, Matrix<Unit> &B, UList &Tau) const
{
    assert(trial<Phi.trials);
    assert(B.rows==1+Phi.K*Phi.neurones);
    assert(B.cols==Phi.neurones);
    const size_t    j        = trial;
    const size_t    neurones = Phi.neurones;
    const PHI::row &Phi_j    = Phi[j];
    const size_t    K        = Phi.K;

    for(size_t i=0;i<neurones;++i)
    {
        const PHI_Functions &phi_j_i = *Phi_j[i];
        size_t               indx    = 0;

        extract(Tau,phi_j_i.train);
        B(0,i) += Tau.size();
        for(size_t l=0;l<neurones;++l)
        {
            const CPW_Functions &phi = *Phi_j[l];
            for(size_t k=0;k<K;++k)
            {
                ++indx;
                B(indx,i) += phi[k].sumValuesAtOrdered(Tau);
            }
        }
    }

}


void Box:: computeMATRIX(const PHI &Phi, Matrix<Unit> &G) const
{
    assert(trial<Phi.trials);
    assert(G.cols == G.rows );
    assert(G.rows == 1+Phi.K*Phi.neurones);

    G(0,0) = tauFinal-tauStart+1;

    const size_t    j        = trial;
    const size_t    neurones = Phi.neurones;
    const PHI::row &Phi_j    = Phi[j];
    const size_t    K        = Phi.K;

    // linear terms
    size_t indx = 0;
    for(size_t i=0;i<neurones;++i)
    {
        const PHI_Functions &phi_j_i = *Phi_j[i];
        for(size_t k=0;k<K;++k)
        {
            const CPW_Function &phi_j_i_k = phi_j_i[k];
            ++indx;
            G(0,indx) = G(indx,0) = phi_j_i_k.integrate(tauStart,tauFinal);
        }
    }

    // mixed terms
    const size_t nm =  Phi.mixed.size;
    const Mix   *pg = &Phi.mixed[0];
    for(size_t ii=0;ii<nm;++ii)
    {
        const Mix          gm(pg[ii]);
        const CPW_Function &phi_j_k = (*Phi_j[gm.i])[gm.k];
        const CPW_Function &phi_l_m = (*Phi_j[gm.l])[gm.m];
        const CPW_Function  p(phi_j_k,phi_l_m);
        //std::cerr << gm << std::endl;
        G(gm.I_i_k,gm.I_l_m) =
        G(gm.I_l_m,gm.I_i_k) = p.integrate(tauStart,tauFinal);
    }
    std::cerr << std::endl;

}


void Box:: appendLinearTo(Matrix<Unit> &G, const PHI &Phi) const
{
    assert(trial<Phi.trials);
    assert(G.cols == G.rows );
    assert(G.rows == 1+Phi.K*Phi.neurones);

    G(0,0) += (tauFinal-tauStart+1);

    const size_t    j        = trial;
    const size_t    neurones = Phi.neurones;
    const PHI::row &Phi_j    = Phi[j];
    const size_t    K        = Phi.K;

    // linear terms
    size_t indx = 0;
    for(size_t i=0;i<neurones;++i)
    {
        const PHI_Functions &phi_j_i = *Phi_j[i];
        for(size_t k=0;k<K;++k)
        {
            const CPW_Function &phi_j_i_k = phi_j_i[k];
            ++indx;
            const Unit ans = phi_j_i_k.integrate(tauStart,tauFinal);
            G(indx,0) = (G(0,indx) += ans);
        }
    }

}

#if 0
void Box:: appendMixedTo(Matrix<Unit>       &G,
                         const CPW_Function &F,
                         const Mix          &g)
{
    const Unit   ans = F.integrate(tauStart,tauFinal);
    const size_t I   = g.I_i_k;
    const size_t J   = g.I_l_m;
    G(I,J) = ( G(J,I) += ans );
}
#endif


////////////////////////////////////////////////////////////////////////////////
Boxes:: ~Boxes() throw()
{
}

Boxes:: Boxes(const Real usr_scale, const size_t nboxes) :
Object(usr_scale),
_Boxes(nboxes),
bmap(),
prod(0),
kMix(this,&Boxes::evalMixed)
{
}


void Boxes:: allocateProducts(const size_t count,const size_t np)
{
    prod.resize(count);
    for(size_t i=0;i<count;++i)
    {
        prod.append<size_t>(np);
    }
}

#include "yocto/exception.hpp"

void Boxes:: mapBoxesPerTrial(const size_t trials)
{
    bmap.free();
    for(size_t i=0;i<size;++i)
    {
        Box *pB = & ((*this)[i]);
        const size_t trial = pB->trial;
        if(trial>=trials)
            throw exception("Box[%u]: invalid trial #%u", unsigned(i+1), unsigned(trial));
        bmap.insert(trial,pB);
    }
    std::cerr << "#BoxGroups=" << bmap.keys() << std::endl;
    for(j=0;j<trials;++j)
    {
        pGrp = bmap.search(j);
        if(pGrp)
        {
            std::cerr << "Boxes for trial #" << j << " : " << pGrp->size << std::endl;
            for(const BoxNode *node = pGrp->head;node;node=node->next)
            {
                const Box &b = *(node->get());
                std::cerr << "\t" << b << std::endl;
            }
        }
    }
}

void Boxes:: updateMixed(const PHI     &Phi,
                         Crew          *para)
{
    //
    pPHI = &Phi;

    //
    const size_t maxProductCount = 2*Phi.maxCount;
    // preparing stuff
    mapBoxesPerTrial(Phi.trials);

    if(para)
    {
        allocateProducts(para->size,maxProductCount);
        for(j=0;j<Phi.trials;++j)
        {
            pGrp = bmap.search(j);
            if(!pGrp) continue;
            (*para)(kMix);
        }
    }
    else
    {
        allocateProducts(1,maxProductCount);
        Crew::single_context ctx;
        for(j=0;j<Phi.trials;++j)
        {
            pGrp = bmap.search(j);
            if(!pGrp) continue;
            kMix(ctx);
        }

    }



#if 0
    // start finding functions
    const size_t    j     = trial;
    const PHI::row &Phi_j = Phi[j];

    if(para)
    {
        const size_t par_size = para->size;
        allocateProducts(par_size,np);
    }
    else
    {
        // sequential code
        allocateProducts(1,np);
        const Mixed  &mixed = Phi.mixed;
        const size_t  nm    = mixed.size;
        CPW_Function &F     = prod[0];
        for(size_t i=0;i<nm;++i)
        {
            const Mix m(mixed[i]);
            const CPW_Function &phi_j_k = (*Phi_j[gm.i])[gm.k];
            const CPW_Function &phi_l_m = (*Phi_j[gm.l])[gm.m];
            F.product(phi_j_k,phi_l_m);
            const Unit ans = F.integrate(tauStart,tauFinal);
        }

    }
#endif
    
}


void Boxes:: evalMixed(Context &ctx)
{
    assert(pPHI);
    assert(pGrp);

    // prepare data
    const PHI      &Phi    = *pPHI; assert(j<Phi.trials);
    const PHI::row &Phi_j  = Phi[j];
    CPW_Function   &F      = prod[ctx.rank];

    // prepare splitting
    const Mixed    &mixed  = Phi.mixed;
    size_t          offset = 0;
    size_t          length = mixed.size;
    ctx.split(offset,length);

    // loop over functions
    for(size_t i=offset,ii=0;ii<length;++i,++ii)
    {
        const Mix           mix(mixed[i]);
        const CPW_Function &phi_j_k = (*Phi_j[mix.i])[mix.k];
        const CPW_Function &phi_l_m = (*Phi_j[mix.l])[mix.m];
        F.product(phi_j_k,phi_l_m);

        //loop over boxes using this trial
        for(const BoxNode *node = pGrp->head; node; node=node->next)
        {
            const Box &box = * (node->get());
            assert(box.trial==j);
            const Unit ans = F.integrate(box.tauStart,box.tauFinal);
            (void)ans;
        }
    }
}


