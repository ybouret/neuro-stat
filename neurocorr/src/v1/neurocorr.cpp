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

    // transfert data
    for(size_t i=0; i < numTrains; ++i)
    {
        RArray<double> &arr = ND->get_raw_input(i);
        for(size_t j=0;j<nc;++j)
        {
            arr[j] = RND[j][i];
        }
    }

    // done
    return ND.yield();
}

extern "C"
SEXP NeuroCorr_CheckNeuroData(SEXP _neuroData, SEXP _numNeurons )
{
    try
    {
        auto_ptr<NeuroData> ND( CreateNeuroData(_neuroData,_numNeurons) );

        return R_NilValue;
    }
    catch(...)
    {
        return R_NilValue;
    }
}
