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
neurones(cols)
{
    _Records &self      = *this;
    unsigned  iTrain    = 0;
    const int maxSpikes = data.cols-1;
    for(size_t j=0;j<trials;++j)
    {
        for(size_t i=0;i<neurones;++i)
        {
            const int numSpikes = int(data(iTrain,0));
            if(numSpikes<0||numSpikes>maxSpikes)
            {
                throw exception("Records: invalid train #%d: #spikes=%d\n", iTrain, numSpikes);
            }
            Train *tr = new Train(numSpikes);
            self[j][i].reset(tr);
            Train &train = *tr;

            // convert
            for(int k=1;k<=numSpikes;++k)
            {
                train[k-1] = toUnit(data(iTrain,k));
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

}

void Records:: display() const
{
    std::cerr << "#trials   = " << trials << std::endl;;
    std::cerr << "#neurones = " << neurones << std::endl;
    const _Records &self = *this;
    for(size_t j=0;j<trials;++j)
    {
        std::cerr << "\ttrial #" << j << std::endl;
        for(size_t i=0;i<neurones;++i)
        {
            const TrainPtr &pTr = self[j][i];
            assert(pTr.is_valid());
            const Train &train = *pTr;
            std::cerr << "\t\tneurone #" << i << " : #spikes=" << train.size() << std::endl;
        }
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

