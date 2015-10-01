#include "boxes-ops.hpp"

#include "boxes-linear.hpp"
#include "boxes-intg.hpp"
#include "boxes-mixed.hpp"

void BoxesOps:: ComputeFor(const Boxes    &boxes,
                           const PHI      &Phi,
                           Matrices       &B,
                           Matrices       &G,
                           Crew           *para)
{
    
    //Linear Terms
    LinearEvaluator   L_Eval(boxes,Phi,B,para);
    
    //Integral Terms of G
    IntegralEvaluator I_Eval(boxes,Phi,G,para);
    
    //Mixed Terms of G
    MixedEvaluator   M_eval(boxes,Phi,G,para);
    
}
