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
