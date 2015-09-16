#include "yocto/R/R++.hpp"
#include "neurodata.hpp"
#include "yocto/ptr/auto.hpp"

using namespace yocto;

////////////////////////////////////////////////////////////////////////////////
//
// C++ interface to R
//
////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
//
// Used to check I/O
//______________________________________________________________________________
extern "C"
SEXP NeuroCorr_Version()
{
    Rprintf("Compiled on %s\n", __DATE__);
    return R_NilValue;
}


static inline
NeuroData *CreateNeuroData(SEXP & _neuroData, SEXP & _numNeurons )
{
    // convert R code into C++ code
    RMatrix<double> RND( _neuroData );

    // get numNeurons
    const int       numNeurons = R2Scalar<int>(_numNeurons);
    if(numNeurons<=0)
        throw exception("numNeurons=%d",numNeurons);

    // get numTrains from data
    const size_t    numTrains  = RND.rows;
    if(numTrains<=0)
        throw exception("no spike train");

    // check compatibility
    if( 0 != (numTrains%numNeurons) )
        throw exception("#Trains(=%u)%%#Neurons(=%d) != 0", unsigned(numTrains), numNeurons);

    // deduce numTrials
    const size_t numTrials = numTrains/numNeurons;

    // build an empty NeuroData object
    const size_t nc = RND.cols;
    auto_ptr<NeuroData> ND( new NeuroData(numNeurons,numTrials,nc-1) );

    // transfer data
    for(size_t i=0; i < numTrains; ++i)
    {
        RArray<double> &arr = ND->get_raw_input(i);
        for(size_t j=0;j<nc;++j)
        {
            arr[j] = RND[j][i];
        }
    }

    Rprintf("[NeuroCorr]: #neurons= %6d\n", int(ND->neurons));
    Rprintf("[NeuroCorr]: #trials = %6d\n", int(ND->trials) );
    Rprintf("[NeuroCorr]: #max_spt= %6d\n", int(ND->max_spikes_per_train) );

    ND->setup();

    // done
    return ND.yield();
}

extern "C"
SEXP NeuroCorr_CheckNeuroData(SEXP _neuroData, SEXP _numNeurons )
{
    YOCTO_R_PROLOG()
    {
        auto_ptr<NeuroData> ND( CreateNeuroData(_neuroData,_numNeurons) );

        for(size_t i=0;i<ND->neurons;++i)
        {
            const Neuron &n = ND->neuron[i];
            Rprintf("neuron[%6d]: #trials=%d\n", int(i), int(n.trials) );
            for(size_t j=0;j<n.trials;++j)
            {
                const Record &tr = n[j];
                Rprintf("      |_trial#%6d: %d\n", int(j), int(tr.size()));
            }
        }


        return R_NilValue;
    }
    YOCTO_R_EPILOG()
}
