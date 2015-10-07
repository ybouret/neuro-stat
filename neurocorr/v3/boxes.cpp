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
        const Box  box(iTrial-1,tauStart,tauFinal,kind);
        push_back(box);
    }
}


#include "yocto/ordered/sorted-vector.hpp"
#include "yocto/associative/map.hpp"

typedef map<Box::Kind,size_t> BoxKindDB;



size_t Boxes:: assignIndices(const Grouping mode) const
{
    static const char fn[] = "Boxes::assignIndices";

    const _Boxes &self = *this;

    switch(mode)
    {
        case GroupByKind:
        {

            // collected all different kinds, SORTED
            sorted_vector<Box::Kind> kvec;
            size_t                   knum=0;

            for(size_t i=0;i<size;++i)
            {
                if(kvec.insert(self[i].kind)) ++knum;
            }

            // sorted association
            BoxKindDB db(knum,as_capacity);
            for(size_t i=1;i<=knum;++i)
            {
                if(!db.insert(kvec[i],i-1)) throw exception("%s: unexpected db.insert failure!",fn);
            }

            // assign indices
            for(size_t i=0;i<size;++i)
            {
                const Box &b = self[i];
                const size_t *pIndx = db.search(b.kind);
                if(!pIndx) throw exception("%s: unexpected missing box kind !!!",fn);
                b.indx = *pIndx;
            }

            return knum;
        }

        case GroupByBox:
        {
            for(size_t i=0;i<size;++i) self[i].indx = i;
            return size;
        }
    }

    throw exception("%s: should never get here!!!",fn);
}
