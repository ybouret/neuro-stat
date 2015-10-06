#include "boxes.hpp"

Boxes:: ~Boxes() throw()
{
}

Boxes:: Boxes(const Real usr_scale, const size_t nboxes) :
Converter(usr_scale),
_Boxes(nboxes)
{
}

#include "yocto/exception.hpp"

Boxes:: Boxes(const Real usr_scale, const Matrix<Real> &RBoxes):
Converter(usr_scale),
_Boxes(RBoxes.cols)
{
    if(RBoxes.rows<4) throw exception("Boxes: need at least 4 rows!");
    for(unsigned c=0;c<RBoxes.cols;++c)
    {
        const int  iTrial   = int(RBoxes(0,c)); if(iTrial<0) throw exception("Box #%u: invalid trial=%d",c,iTrial);
        const Unit tauStart = toUnit(RBoxes(1,c));
        const Unit tauFinal = toUnit(RBoxes(2,c));
        if(tauFinal<=tauStart) throw exception("Box #%u: times are not well separated (check scale=%g)",c,scale);
        const int  kind     = int(RBoxes(3,c));
        const Box  box(iTrial,tauStart,tauFinal,kind);
        push_back(box);
    }
}



void Boxes:: buildDB( BoxKindDB &db ) const
{
    const _Boxes &self = *this;
    db.free();
    for(size_t i=0;i<size;++i)
    {
        const Box::Kind k = self[i].kind;
        if(!db.search(k))
        {
            if(!db.insert(k,db.size())) throw exception("Unexpected Failure in Boxes::buildDB");
        }
    }

    // one matrix per kind
    for(size_t i=0;i<size;++i)
    {
        const Box &b = self[i];
        const size_t *pIndx = db.search(b.kind);
        if(!pIndx) throw exception("Boxes::buildDB:: unexpected missing box kind !!!");
        b.indx = *pIndx;
    }
}



size_t Boxes:: assignIndices(const Grouping mode) const
{
    size_t ans = 0;
    switch(mode)
    {
        case GroupByKind:
        {
            BoxKindDB db;
            buildDB(db);
            ans = db.size();
        }
            break;

        case GroupByBox:
        {
            for(size_t i=0;i<size;++i) (*this)[i].indx = i;
            ans = size;
        }
            break;
    }
    return ans;
}
