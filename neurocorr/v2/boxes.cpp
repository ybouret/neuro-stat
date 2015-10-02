#include "boxes.hpp"

Boxes:: ~Boxes() throw()
{
}

Boxes:: Boxes(const Real usr_scale, const size_t nboxes) :
Object(usr_scale),
_Boxes(nboxes)
{
}

#include "yocto/exception.hpp"

void Boxes:: buildDB( Box::KindDB &db ) const
{
    db.free();
    for(size_t i=0;i<size;++i)
    {
        const Box::Kind k = (*this)[i].kind;
        if(!db.search(k))
        {
            if(!db.insert(k,db.size())) throw exception("Unexpected Failure in Boxes::buildDB");
        }
    }

    // one matrix per kind
    for(size_t i=0;i<size;++i)
    {
        const Box &b = (*this)[i];
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
            Box::KindDB db;
            buildDB(db);
            ans = db.size();
        }

        case GroupByBox:
        {
            for(size_t i=0;i<size;++i) (*this)[i].indx = i;
            ans = size;
        }
    }
    return ans;
}
