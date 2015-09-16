#include "trial.hpp"

Trial:: ~Trial() throw() {}

Trial:: Trial(const size_t N) :
Records(N),
neurons(size)
{
}


Trials:: ~Trials() throw()
{
}

Trials:: Trials(const size_t numTrials) : TrialsBase(numTrials) {}
