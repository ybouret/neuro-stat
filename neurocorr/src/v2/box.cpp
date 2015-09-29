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






