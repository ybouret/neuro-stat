#include "yocto/R++/com.hpp"
#include "records.hpp"
using namespace yocto;

YOCTO_R_FUNCTION(NeuroCorr_Version,())
{
    Rprintf("%s: compiled on %s\n", __fn, __DATE__ );
    return R_NilValue;
}
YOCTO_R_RETURN()


static size_t NumThreads = 0;

#include "yocto/sys/hw.hpp"

YOCTO_R_FUNCTION(NeuroCorr_SetParallel,(SEXP numProcsR))
{
    NumThreads = 0;
    int numProcs = R2Scalar<int>(numProcsR);
    if(numProcs>0)
    {
        const int maxProcs = hardware::nprocs();
        if(numProcs>maxProcs)
        {
            numProcs=maxProcs;
        }
        NumThreads = numProcs;
    }
    return R_NilValue;

}
YOCTO_R_RETURN()


YOCTO_R_FUNCTION(NeuroCorr_CheckData,(SEXP dataNeurR, SEXP numNeuronesR, SEXP scaleR) )
{
    RMatrix<Real> dataNeur(dataNeurR);
    const int     numNeurones = R2Scalar<int>(numNeuronesR);
    const Real    scale       = R2Scalar<double>(scaleR);

    std::cerr << "#neurones=" << numNeurones << std::endl;
    std::cerr << "Scale    =" << scale       << std::endl;

    const Records records(scale,dataNeur,numNeurones);
    records.display();

    return R_NilValue;
}
YOCTO_R_RETURN()