#include "box.hpp"


Box:: ~Box() throw()
{
}

Box:: Box(const size_t iTrial,
          const Unit   ini,
          const Unit   end,
          const Kind   aka) throw() :
trial(iTrial),
tauStart(ini),
tauFinal(end),
kind(aka),
indx(0)
{
    assert(tauFinal>=tauStart);
    assert(trial>0);
}


Box:: Box(const Box &b) throw() :
trial(b.trial),
tauStart(b.tauStart),
tauFinal(b.tauFinal),
kind(b.kind),
indx(b.indx)
{

}
