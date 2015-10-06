#include "records.hpp"

Records:: ~Records() throw()
{
}

#include "yocto/exception.hpp"


static inline
size_t __chkTrials(const size_t numNeurones, const size_t numTrains)
{
    if(numTrains<=0)
    {
        throw exception("Records: no train!");
    }
    if(numNeurones<=0)
    {
        throw exception("Records: no neurone!");
    }
    if( 0 != (numTrains%numNeurones) )
    {
        throw exception("Records: mismatch dimensions !");
    }

    const size_t numTrials = numTrains/numNeurones;
    if(numTrials<=0)
        throw exception("Records: too many neurones");

    return numTrials;
}



Records:: Records(const Real          usrScale,
                  const Matrix<Real> &data,
                  const size_t        numNeurones) :
Converter(usrScale),
_Records(__chkTrials(numNeurones,data.rows),numNeurones),
trials(rows),
neurones(cols),
maxCount(0),
tauMin(0),
tauMax(0)
{
    build_with<size_t>(0);
    _Records &self      = *this;
    unsigned  iTrain    = 0;
    const int maxSpikes = data.cols-1;
    size_t count  = 0;
    Unit   tMin   = 0;
    Unit   tMax   = 0;
    bool   init   = true;

    for(size_t i=0;i<neurones;++i)
    {
        for(size_t j=0;j<trials;++j)
        {
            const int numSpikes = int(data(iTrain,0));
            if(numSpikes<0||numSpikes>maxSpikes)
            {
                throw exception("Records: invalid train #%d: #spikes=%d\n", iTrain, numSpikes);
            }

            Train &train = self[j][i];
            {
                _Train tmp(numSpikes);
                train.swap_with(tmp);
            }
            assert(unsigned(numSpikes)==train.size());
            
            // convert
            for(int k=1;k<=numSpikes;++k)
            {
                train[k-1] = toUnit(data(iTrain,k));
            }

            count = max_of<size_t>(count, numSpikes);
            if(numSpikes>0)
            {
                if(init)
                {
                    tMin = train[0];
                    tMax = train[numSpikes-1];
                    init = false;
                }
                else
                {
                    tMin = min_of(tMin,train[0]);
                    tMax = max_of(tMax,train[numSpikes-1]);
                }
            }

            // check orderd
            for(int k=1;k<numSpikes;++k)
            {
                if(train[k-1]>=train[k])
                {
                    throw exception("Records: invalid coordinate for train #%d (check scaling...)", iTrain);
                }
            }
            ++iTrain;
        }
    }

    (size_t&)maxCount = count;
    (Unit  &)tauMin   = tMin;
    (Unit  &)tauMax   = tMax;
    
}

void Records:: display() const
{
    std::cerr << "#trials   = " << trials << std::endl;;
    std::cerr << "#neurones = " << neurones << std::endl;
    std::cerr << "#maxCount = " << maxCount << std::endl;
    std::cerr << "tauMin    = " << tauMin   << std::endl;
    std::cerr << "tauMax    = " << tauMax   << std::endl;

    const _Records &self = *this;
    for(size_t j=0;j<trials;++j)
    {
        std::cerr << "\ttrial #" << j+1;
        for(size_t i=0;i<neurones;++i)
        {
            const Train &train = self[j][i];
            std::cerr << "/@" << i+1 << ":" << train.size();
        }
        std::cerr << std::endl;
    }
}


#include "yocto/code/rand.hpp"

Records * Records:: CreateRandom(const size_t numTrials,
                                 const size_t numNeurones,
                                 const size_t maxSpikes,
                                 const size_t pace)
{

    assert(numTrials>0);
    assert(numNeurones>0);

    const size_t  numTrains = numTrials*numNeurones;
    CMatrix<Real> data(numTrains,1+maxSpikes);
    for(size_t i=0;i<numTrains;++i)
    {
        const size_t numSpikes = alea_leq(maxSpikes);
        data(i,0) = numSpikes;
        Unit curr = alea_leq(pace);
        for(size_t k=1;k<=numSpikes;++k)
        {
            curr += 1+alea_leq(pace);
            data(i,k) = curr;
        }
    }
    
    return new Records(1.0,data,numNeurones);
}

