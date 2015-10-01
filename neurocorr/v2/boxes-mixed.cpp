#include "boxes.hpp"
#include "yocto/exception.hpp"


MixedEvaluator:: ~MixedEvaluator() throw()
{

}


MixedEvaluator:: MixedEvaluator(const Boxes       &boxes,
                                const PHI         &UsrPhi,
                                Matrices          &Gmatrices,
                                Crew              *para) :
Phi(UsrPhi),
trials(Phi.trials),
J(0),
mgr(trials),
prod(para?para->size:1),
G(Gmatrices)
{

    Crew::single_context mono;

    // allocate memory for products
    {
        const size_t nt = prod.capacity;
        const size_t np = 2*Phi.maxCount;
        for(size_t i=0;i<nt;++i) prod.append<size_t>(np);
    }


    // map boxes per trials
    for(size_t j=0;j<trials;++j)
    {
        mgr.push_back();
    }
    assert(mgr.size==trials);
    
    const size_t nb = boxes.size;
    for(size_t i=0;i<nb;++i)
    {
        const Box &box = boxes[i];
        if(box.trial>=trials) throw exception("Invalid Box Trial!");
        mgr[box.trial].append(&box);
    }

    // loop over trials
    Kernel run(this, & MixedEvaluator::compute);
    for(J=0;J<trials;++J)
    {
        const BoxList &bl = mgr[J];
        if(bl.size)
        {
            if(para)
            {
                (*para)(run);
            }
            else
            {
                run(mono);
            }
        }
    }

}


void MixedEvaluator::compute(Context &ctx)
{
    //__________________________________________________________________________
    //
    // prepare data
    //__________________________________________________________________________
    assert(J<trials);
    const PHI::row &Phi_j  = Phi[J];
    CPW_Function   &F      = prod[ctx.rank]; //!< one temporary per thread, pre-allocated

    //__________________________________________________________________________
    //
    // prepare splitting
    //__________________________________________________________________________
    const Mixed    &mixed  = Phi.mixed;
    size_t          offset = 0;
    size_t          length = mixed.size;
    ctx.split(offset,length);

    //__________________________________________________________________________
    //
    // loop over functions
    //__________________________________________________________________________
    for(size_t i=offset,ii=0;ii<length;++i,++ii)
    {
        const Mix           mix(mixed[i]);
        const CPW_Function &phi_j_k = (*Phi_j[mix.i])[mix.k];
        const CPW_Function &phi_l_m = (*Phi_j[mix.l])[mix.m];
        F.product(phi_j_k,phi_l_m);
        const size_t Indx = mix.I_i_k; //!< accessible only for this thread
        const size_t Jndx = mix.I_l_m; //!< accessible only for this thread
        
        //______________________________________________________________________
        //
        // loop over boxes using this trial
        //______________________________________________________________________
        for(const BoxNode *node=mgr[J].head;node;node=node->next)
        {
            const Box    *box = node->addr; assert(box); assert(box->trial==J);
            const size_t   g  = box->indx;  assert(g<G.size);
            const Unit    ans = F.integrate(box->tauStart,box->tauFinal);
            Matrix<Real>  &GG = *G[g];
            GG(Indx,Jndx) = ( GG(Jndx,Indx) += ans);
        }

    }


}



