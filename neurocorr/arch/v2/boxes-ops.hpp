#ifndef BOXES_OPS_INCLUDED
#define BOXES_OPS_INCLUDED 1

#include "boxes.hpp"

//! compute all for prepared boxes
struct BoxesOps
{
    static void ComputeFor(const Boxes    &boxes,
                           const PHI      &Phi,
                           Matrices       &B,
                           Matrices       &G,
                           Crew           *para);
};


#endif

