#include "boxes.hpp"


Boxes:: ~Boxes() throw()
{
}

Boxes:: Boxes(const Real            usrScale ,
              const matrix_of<Real> boxes) :
Converter(usrScale),
_Boxes( boxes.cols, as_capacity )
{
    if(boxes.rows<4)
        throw exception("Boxes: not enough rows (at least 4 are required)");

    const size_t nb = boxes.cols;

    for(size_t b=1;b<=nb;++b)
    {
        const size_t iTrial   = size_t(floor(boxes(1,b)+0.5));
        if(iTrial<=0)
            throw exception("Box #%u: invalid trial", unsigned(b));

        const Unit   tauStart = toUnit(boxes(2,b));
        const Unit   tauFinal = toUnit(boxes(3,b));
        if(tauFinal>tauStart)
            throw exception("Box #%u: invalid start_time>final_time", unsigned(b) );

        const Box::Kind kind = Box::Kind(boxes(4,b));
        const Box box(iTrial,tauStart,tauFinal,kind);
        __push_back(box);
    }
}

#include "yocto/ordered/sorted-vector.hpp"
#include "yocto/associative/map.hpp"

size_t Boxes:: assignIndices(const Grouping mode) const
{
    switch (mode) {
        case GroupByBox:
        {
            const _Boxes &self = *this;
            const size_t nb    = self.size();
            for(size_t i=1;i<=nb;++i)
            {
                self[i].indx = i;
            }
            return nb;
        }

        case GroupByKind:
        {
            const _Boxes            &self = *this;
            const size_t             nb    = self.size();
            sorted_vector<Box::Kind> kinds(nb,as_capacity);

            // find all different kinds, SORTED
            for(size_t i=1;i<=nb;++i)
            {
                const Box::Kind kind = self[i].kind;
                if(!kinds.search(kind))
                {
                    if(!kinds.insert(kind)) throw exception("unexpected insertion failure in Boxes::assignIndices");
                }
            }

            // make a database
            const size_t count = kinds.size();
            map<Box::Kind,size_t> db(count,as_capacity);
            for(size_t i=1;i<=count;++i)
            {
                if(!db.insert(kinds[i],i)) throw exception("unexpected insertion failure in boxes kind database");
            }

            // assign indices
            for(size_t i=1;i<=nb;++i)
            {
                const Box    &box   = self[i];
                const size_t *pIndx = db.search(box.kind);
                if(!pIndx) throw exception("unexpected no index for box behavior!!!");
                box.indx = *pIndx;
            }
            
            return count;
        }
    }

    throw exception("Boxes: assignIndices unexpected failure!");
    return 0;
}
