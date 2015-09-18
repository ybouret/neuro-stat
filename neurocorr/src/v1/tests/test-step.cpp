#include "../step-function.hpp"
#include "yocto/utest/run.hpp"
#include "yocto/ios/ocstream.hpp"
#include "yocto/code/rand.hpp"

YOCTO_UNIT_TEST_IMPL(step)
{
    StepFunction F;

    for(size_t i=1+alea_leq(10);i>0;--i)
    {
        F.insert( alea<double>(), (alea<double>()-0.5) * 10.0 );
    }

    F.head = alea<double>();
    F.tail = -alea<double>();

    ios::wcstream fp("step.dat");
    for(double t=0;t<=1;t+=0.001)
    {
        fp("%g %g\n", t, F(t));
    }
}
YOCTO_UNIT_TEST_DONE()
