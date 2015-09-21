#include "yocto/R/R++.hpp"
#include "neuron.hpp"
#include "yocto/ptr/auto.hpp"

using namespace yocto;

////////////////////////////////////////////////////////////////////////////////
//
// C++ interface to R
//
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
//
//
// Used to check I/O
//
//______________________________________________________________________________
extern "C"
SEXP NeuroCorr_Version() throw()
{
    Rprintf("Compiled on %s\n", __DATE__);
    return R_NilValue;
}


//______________________________________________________________________________
//
//
// Prototype to see if data is well loaded
//
//______________________________________________________________________________
static inline
Neurons *BuildNeurons(SEXP &RND, SEXP &RNumNeurons, SEXP &RScale)
{
    //______________________________________________________________________
    //
    // Parse Arguments
    //______________________________________________________________________
    RMatrix<Real> neurodata(RND);
    const int     num_neurons = R2Scalar<int>(RNumNeurons);
    const Real    scale       = R2Scalar<Real>(RScale);
    
    //______________________________________________________________________
    //
    // Direct Call
    //______________________________________________________________________
    return new Neurons(scale,neurodata,num_neurons);
}

extern "C"
SEXP NeuroCorr_CheckNeuroData(SEXP RND, SEXP RNumNeurons, SEXP RScale) throw()
{
    YOCTO_R_PROLOG()
    {

        auto_ptr<Neurons> pNeurons( BuildNeurons(RND,RNumNeurons,RScale) );
        Neurons &neurons = *pNeurons;
        Rprintf("#neurons=%u (#trials=%u)\n", unsigned(neurons.size), unsigned(neurons.trials) );
        for(int i=0;i<neurons.size;++i)
        {
            const Neuron &nn = neurons[i];
            Rprintf("Neuron[%3d]:\n", i);
            for(unsigned j=0;j<nn.trials;++j)
            {
                const Train &tr = nn[j];
                Rprintf("\ttrial[%3u](#%3u)",j,tr.size());
                Rprintf("\n");
            }

        }
        return R_NilValue;
    }
    YOCTO_R_EPILOG()
}