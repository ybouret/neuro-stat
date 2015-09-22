#include "phi.hpp"

Phi:: ~Phi() throw()
{
}

Phi:: Phi(const size_t extra,
          const Train &train,
          const Unit   deltaUnit) :
PhiBase(1+extra)
{
    
}
