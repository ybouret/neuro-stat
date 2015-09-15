#ifndef NC_RECORD_INCLUDED
#define NC_RECORD_INCLUDED 1


#include "yocto/sequence/lw-array.hpp"

using namespace yocto;

//! a single record
class Record : public lw_array<double>
{
public:
    //! extract data
    explicit Record(array<double> &data);
    virtual ~Record() throw();
    
private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(Record);
};

#include "yocto/container/slots.hpp"

class Records : public slots_of<Record>
{
public:
    explicit Records(size_t M);
    virtual ~Records() throw();

    void Append( array<double> &data );
    
private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(Records);
};

#endif
