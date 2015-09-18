#include "step-function.hpp"


int CompareCoords(const Coord &lhs, const Coord &rhs) throw()
{
    return __compare(lhs.t,rhs.t);
}

StepFunction:: ~StepFunction() throw()
{
}


size_t StepFunction::size() const throw()
{
    return coords.size();
}

StepFunction:: StepFunction(size_t n) :
foot(0),
coords(n,as_capacity)
{

}


void StepFunction:: insert(const double t, const double v)
{
    // append the coordinate
    {
        const Coord C(t,v);
        coords.push_back(C);
    }
    
    //check its place
    for(size_t j=coords.size(),jm=j-1;jm>0;--j,--jm)
    {
        Coord &Cm = coords[jm];
        Coord &C  = coords[j];
        if(CompareCoords(Cm,C)>0)
        {
            bswap(Cm,C);
        }
        else
        {
            break;
        }
    }
   
}


size_t StepFunction:: find_index_(const double t) const throw()
{
    assert(t>coords.front().t);
    assert(t<=coords.back().t);
    size_t jlo = 1;
    size_t jhi = size();
    while(jhi-jlo>1)
    {
        const size_t jmid = (jlo+jhi)>>1;
        const double tmid = coords[jmid].t;
        if(t<=tmid)
        {
            jhi = jmid;
        }
        else
        {
            if(t>tmid)
            {
                jlo = jmid;
            }
            else
            {
                return jmid; // special numeric case, t==tmid
            }
        }
    }
    return jlo;

}


size_t StepFunction:: find_index(const double t) const throw()
{
    const size_t n = coords.size();
    assert(n>0);
    const Coord &C1 = coords.front();
    if(t<=C1.t)
    {
        return 0;
    }
    else
    {
        const Coord &Cn = coords.front();
        if(t>Cn.t)
        {
            return n;
        }
        else
        {
            return find_index_(t);
        }
    }

}

double StepFunction:: operator()(const double t) const throw()
{
    if(t<=coords.front().t)
    {
        return foot;
    }
    else
    {
        const Coord &final = coords.back();
        if(t>final.t)
        {
            return final.v;
        }
        else
        {
            return coords[ find_index_(t) ].v;
        }
    }
}

const Coord & StepFunction:: operator[](const size_t indx) const throw()
{
    assert(indx>0);
    assert(indx<=size());
    return coords[indx];
}


void StepFunction:: buildFromFull( const Record &train, const double delta)
{
    assert(delta>0);
    std::cerr << "Building Step Function for #spikes=" << train.size() << std::endl;
    const size_t num_spikes = train.size();
    const size_t num_clicks = 2*num_spikes;

    // make a copy
    CVector<double> shift(num_spikes); assert(train.size()==shift.size());
    for(size_t i=0;i<num_spikes;++i)
    {
        shift[i] = train[i] + delta;
    }

    // prepare data
    coords.free();
    coords.ensure(num_clicks);
    foot = 0;

    // optimized fusion sort
    int    curr   = 0;
    size_t iTrain = 0;
    size_t iShift = 0;


    while(iTrain<num_spikes)
    {
        const double tt = train[iTrain];
        const double ts = shift[iShift];
        if(tt<ts)
        {
            ++curr;
            const Coord C(tt,curr);
            coords.push_back(C);
            ++iTrain;
        }
        else
        {
            --curr;
            const Coord C(ts,curr);
            coords.push_back(C);
            ++iShift;
        }
    }

    // end fusion with remaining shift
    while(iShift<num_spikes)
    {
        --curr;
        const Coord C(shift[iShift++],curr);
        coords.push_back(C);
    }



}

