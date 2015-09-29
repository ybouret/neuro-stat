#include "boxes.hpp"

void Boxes:: updateLinearVectors(const PHI &Phi)
{
    pPHI = &Phi;
    for(size_t i=0;i<size;++i)
    {
        const Box &box = (*this)[i];
        J = box.trial;
        
    }
}
