#include "matrix-builder.hpp"

MatrixBuilder:: ~MatrixBuilder() throw()
{

}


MatrixBuilder:: MatrixBuilder(Matrices<Unit> &matrices,
                              const Boxes    &boxes,
                              const PHI      &usrPhi,
                              Crew           *para) :
MG(matrices),
Phi(usrPhi),
mgr(Phi.trials)
{
    //build manager
    const size_t trials = Phi.trials;
    for(size_t i=0;i<trials;++i)
    {
        mgr.push_back();
    }

    //record boxes per trials
    const size_t nb = boxes.size;
    for(size_t b=0;b<nb;++b)
    {
        const Box *box = &boxes[b];
        assert(box->trial<trials);
        mgr[box->trial].append(box);
    }

    

}
