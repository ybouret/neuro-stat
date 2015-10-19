#include "records.hpp"

Records::~Records() throw()
{
}

Records:: Records(const Real             scal,
                  const matrix_of<Real> &data,
                  const size_t           numNeurones) :
Converter(scal),
_Records(),
trials(rows),
neurones(cols)
{
    static const char __fn[] = "Records";

    if(numNeurones<=0)             throw exception("%s: numNeurones=0",__fn);
    if(data.cols<1)                throw exception("%s: No Column In NeuroData",__fn);
    if(data.rows<numNeurones)      throw exception("%s: not enough trains!",__fn);
    if(0!=(data.rows%numNeurones)) throw exception("%s: #trains=%u not multiple of #neurones=%u", __fn, unsigned(data.rows), unsigned(numNeurones));

    const size_t numTrials = data.rows/numNeurones;

    make(numTrials,numNeurones);
    assert(trials==numTrials);
    assert(neurones==numNeurones);

    const size_t max_spikes = data.cols-1;
    _Records &self = *this;
    size_t    tr   = 0;
    for(size_t j=1;j<=numTrials;++j)
    {
        for(size_t i=1;i<=numNeurones;++i)
        {
            ++tr;
            const size_t ns = data(tr,1);
            if(ns>max_spikes)
                throw exception("%s: train #%u: too many spikes!", __fn, unsigned(tr));
            Train &train = self[j][i];
            train.make(ns);
            for(size_t k=1,c=2;k<=ns;++k,++c)
            {
                train[k] = toUnit( data(tr,c) );
            }
            assert(train.size()==ns);
            for(size_t k=2;k<=ns;++k)
            {
                if(train[k]<=train[k-1])
                    throw exception("%s: train #%u: (time[%u]=%g->%ld)>=(time[%u]=%g->%ld), check scaling or data...",
                                    __fn,
                                    unsigned(tr),
                                    unsigned(k+1),
                                    data(tr,k+1),
                                    long(train[k]),
                                    unsigned(k),
                                    data(tr,k),
                                    long(train[k-1]));
            }
        }
    }


}

void Records:: display() const
{

    const _Records &self = *this;
    for(size_t i=1;i<=neurones;++i)
    {
        std::cerr << "neurone " << i << "/" << neurones  << "#trials=" << trials;
        for(size_t j=1;j<=trials;++j)
        {
            std::cerr << ':' << self[j][i].size();
        }
        std::cerr << std::endl;
    }

}

#include "yocto/code/rand.hpp"

Records *Records:: GenerateRandom(const size_t num_neurones,
                                  const size_t num_trials,
                                  const size_t max_spikes,
                                  const size_t pace)
{

    assert(num_neurones>0);
    assert(num_trials>0);
    const size_t num_trains = num_trials*num_neurones;
    matrix<Real> nd(num_trains,1+max_spikes);

    for(size_t i=1;i<=num_trains;++i)
    {
        const size_t ns = alea_leq(max_spikes);
        nd[i][1]  = ns;
        Unit curr = 0;
        for(size_t j=1;j<=ns;++j)
        {
            curr += 1 + Unit(alea_lt(pace));
            nd[i][j+1] = curr;
        }
    }
    return new Records(1.0,nd,num_neurones);
}