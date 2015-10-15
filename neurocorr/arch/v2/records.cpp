#include "records.hpp"

Records:: ~Records() throw()
{

}

#include "yocto/exception.hpp"

static inline
size_t __get_num_trials(const size_t trains, const size_t num_neurons)
{
    if(trains<=0)                   throw exception("Records: No Train");
    if(num_neurons<=0)              throw exception("Records: No Neurone");
    if(trains<num_neurons)          throw exception("Records: Mismatch Trains<Neurones");
    if( 0 != (trains%num_neurons) ) throw exception("Records: Incompatible Lengths");
    return trains/num_neurons;
}

#include "yocto/code/utils.hpp"

Records:: Records(const Real          scale,
                  const Matrix<Real> &neurodata,
                  const size_t        num_neurons) :
Object(scale),
RecordsBase( __get_num_trials(neurodata.rows,num_neurons), num_neurons ),
trials(rows),
neurones(cols),
maxCount(0),
tauMin(0),
tauMax(0)
{
    build_with<Train*>(NULL);
    RecordsBase &self = *this;
    size_t iTrain = 0;
    size_t count  = 0;
    Unit   tMin   = 0;
    Unit   tMax   = 0;
    bool   init   = true;
    for(size_t iN = 0; iN < neurones; ++iN )
    {
        for(size_t iT = 0; iT < trials; ++iT )
        {
            Train *tr = new Train(scale,neurodata,iTrain);
            self[iT][iN].reset(tr);
            const size_t trSize = tr->size();
            if(init)
            {
                if(trSize>0)
                {
                    tMin = (*tr)[0];
                    tMax = (*tr)[trSize-1];
                    init = false;
                }
            }
            else
            {
                if(trSize>0)
                {
                    tMin = min_of(tMin,(*tr)[0]);
                    tMax = max_of(tMax,(*tr)[trSize-1]);
                }
            }
            ++iTrain;
            if(trSize>count) count = trSize;
        }
    }
    (size_t&)maxCount = count;
    (size_t&)tauMin   = tMin;
    (size_t&)tauMax   = tMax;
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


#include "yocto/code/rand.hpp"

Records *Records:: CreateRandom(const size_t num_neurones,
                                const size_t num_trials,
                                const size_t max_spikes)
{
    assert(num_neurones>0);
    assert(num_trials>0);
    const size_t  num_trains = num_neurones*num_trials;
    CMatrix<Real> neurodata(num_trains,1+max_spikes);

    for(size_t i=0;i<num_trains;++i)
    {
        const size_t ns = alea_leq(max_spikes);
        neurodata[i][0] = ns;
        size_t curr = 0;
        for(size_t j=1;j<=ns;++j)
        {
            curr += 1 + alea_leq(10);
            neurodata[i][j] = curr;
        }
    }

    return new Records(1.0,neurodata,num_neurones);
}
