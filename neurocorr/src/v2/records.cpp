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
neurones(cols)
{
    build_with<Train*>(NULL);
    RecordsBase &self = *this;
    size_t iTrain = 0;
    for(size_t iN = 0; iN < neurones; ++iN )
    {
        for(size_t iT = 0; iT < trials; ++iT )
        {
            self[iT][iN].reset( new Train(scale,neurodata,iTrain) );
            ++iTrain;
        }
    }
}


void Records:: displayNeurones() const
{
    std::cerr << "#Neurons=" << neurones << std::endl;

    for(size_t j=0;j<neurones;++j)
    {
        std::cerr << "neuron #" << j << "\t(#trials=" << trials << ")" << std::endl;
        std::cerr << "|_";
        for(size_t i=0;i<trials;++i)
        {
            const Train &tr = *((*this)[i][j]);
            std::cerr << "/" << tr.size();
        }
        std::cerr << "." << std::endl;
    }
    
}
