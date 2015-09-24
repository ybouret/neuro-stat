#include "records.hpp"

Records:: ~Records() throw()
{

}

#include "yocto/exception.hpp"

static inline
size_t __get_num_trials(const size_t trains, const size_t num_neurons)
{
    if(trains<=0)                   throw exception("Records: No Train");
    if(num_neurons<=0)              throw exception("Records: No Neuron");
    if(trains<num_neurons)          throw exception("Records: Mismatch Trains<Neurons");
    if( 0 != (trains%num_neurons) ) throw exception("Records: Incompatible Length");
    return trains/num_neurons;
}

Records:: Records(const Real          scale,
                  const Matrix<Real> &neurodata,
                  const size_t        num_neurons) :
RecordsBase( __get_num_trials(neurodata.rows,num_neurons), num_neurons ),
trials(rows),
neurons(cols)
{
    build_with<Train*>(NULL);
    RecordsBase &self = *this;
    size_t iTrain = 0;
    for(size_t iN = 0; iN < neurons; ++iN )
    {
        for(size_t iT = 0; iT < trials; ++iT )
        {
            self[iT][iN].reset( new Train(scale,neurodata,iTrain) );
            ++iTrain;
        }
    }
}
