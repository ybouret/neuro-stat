#include "matrix-builder.hpp"
#include <cstdlib>

MatrixBuilder:: ~MatrixBuilder() throw()
{
}

MatrixBuilder:: MatrixBuilder(matrices_of<Unit> &usrMG,
                              const Boxes       &boxes,
                              const PHI         &usrPhi,
                              threading::crew  *team) :
MG(usrMG),
Phi(usrPhi),
box(0),
JJ(0),
S(Phi.NK),
mgr(Phi.trials),
prod()
{
    assert(MG.rows==Phi.dim);
    assert(MG.cols==Phi.dim);
    assert(Phi.trials==mgr.size());
    const size_t count  = MG.count;
    const size_t trials = Phi.trials;
    const size_t nb     = boxes.size();


    threading::kernel_executor &kExec = team ? *static_cast<threading::kernel_executor*>(team) : Phi.kSeq;

    //__________________________________________________________________________
    //
    // Reserve memory for product functions
    //__________________________________________________________________________
    {
        const size_t num_threads = kExec.num_threads();
        prod.ensure(num_threads);

        const size_t np = Phi.maxCount*2;
        for(size_t i=kExec.num_threads();i>0;--i)
        {
            prod.append<size_t>(np);
        }
        assert(num_threads==prod.size());
    }

    {
        //______________________________________________________________________
        //
        // Loop on boxes to compute sides terms.
        // Register boxes in manager.
        //______________________________________________________________________
        threading::kernel kSide(this,& MatrixBuilder::computeSide);
        for(size_t b=nb;b>0;--b)
        {
            box            = &boxes[b];
            const size_t j = box->trial;
            const size_t m = box->indx;

            if(j<=0||j>trials) throw exception("MatrixBuilder: invalid box #%u trial=%u!", unsigned(b), unsigned(j) );
            if(m<=0||m>count)  throw exception("MatrixBuilder: invalid box #%u index=%u!", unsigned(b), unsigned(m) );

            matrix_of<Unit> &G        = MG[m];
            const Unit       tauStart = box->tauStart;
            const Unit       tauFinal = box->tauFinal;
            if(tauStart>tauFinal)
                throw exception("MatrixBuilder: invalid box #%u times!",unsigned(b));
            G(1,1) += tauFinal-tauStart;
            //! store the address of the box in the manager corresponding pos;
            mgr[j].append(box);
            kExec(kSide);

        }
    }
    


    {
        //______________________________________________________________________
        //
        // Loop over trials
        //______________________________________________________________________
        threading::kernel kCore(this, &MatrixBuilder::computeCore);
        for(JJ=trials;JJ>0;--JJ)
        {
            const BoxList &boxlist = mgr[JJ];
            if(boxlist.size>0)
            {
                kExec(kCore);
            }
        }
    }

}


void MatrixBuilder:: computeSide( threading::context &ctx )
{
    assert(box);
    const size_t j = box->trial;
    const size_t m = box->indx;

    size_t offset = 0;
    size_t length = Phi.NK;
    ctx.split(offset,length);

    const PHI::row  &Phi_j    = Phi[j];
    matrix_of<Unit> &G        = MG[m];
    const size_t     K        = Phi.K;
    const Unit       tauStart = box->tauStart;
    const Unit       tauFinal = box->tauFinal;


    for(size_t r=2+offset;length>0;++offset,++r,--length)
    {
        //______________________________________________________________________
        //
        // get the i,k coordinates corresponding => row/line
        //______________________________________________________________________
        ldiv_t       d = ldiv(offset,K);
        const size_t i = ++d.quot;
        const size_t k = ++d.rem;
        assert(i<=Phi.neurones);
        assert(k<=Phi.K);
        assert((i-1)*K+(k-1) == offset);

        const CPW &phi  = Phi_j[i][k];
        const Unit intg = phi.integrate(tauStart,tauFinal);
        G(r,1) = ( G(1,r) += intg );
    }


}


void MatrixBuilder:: computeCore( threading::context &ctx )
{
    assert(JJ>0);
    assert(JJ<=Phi.trials);
    CPW           &P      = prod[ctx.indx];
    const BoxNode *i_node = mgr[JJ].head;
    size_t         offset = 0;
    size_t         length = S.items;
    ctx.split(offset,length);

    // loop on elements of matrix
    const size_t    K     = Phi.K;
    const PHI::row &Phi_j = Phi[JJ];

    for(size_t Q=offset;length>0;--length,++Q)
    {

        // find position in G matrix, J>=I, 0-based, warning
        size_t I=0,J=0;
        S.FindIJ(Q,I,J);

        // find corresponding ROW neurone and k
        ldiv_t rd = ldiv(I,K);
        const size_t ri = ++rd.quot;
        const size_t rk = ++rd.rem;
        assert(I==(ri-1)*K+(rk-1));

        // find corresponding COLUMN neurone and k
        ldiv_t       cd = ldiv(J,K);
        const size_t ci = ++cd.quot;
        const size_t ck = ++cd.rem;
        assert(J==(ci-1)*K+(ck-1));

        // compute the product
        I+=2;
        J+=2;
        P.product(Phi_j[ri][rk],Phi_j[ci][ck]);

        for(const BoxNode *node=i_node;node;node=node->next)
        {
            const Box *sub = node->addr;
            assert(sub->trial==JJ);
            const Unit       intg = P.integrate(sub->tauStart,sub->tauFinal);
            matrix_of<Unit> &G    = MG[sub->indx];
            G(I,J) = ( G(J,I) += intg );
        }
        
        
    }
    
}

