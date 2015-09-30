#ifndef NC_RECORD_INCLUDED
#define NC_RECORD_INCLUDED 1


#include "yocto/R/IR.hpp"

using namespace yocto;

//! a single record
/**
 - Record.size() : #spikes in the record
 - Record[0..size()-1]: spikes timings
 */
class Record : public LArray<double>
{
public:
    //! extract #spikes and timings from data
    explicit Record(RArray<double> &data);

    //! destructor
    virtual ~Record() throw();
    
private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(Record);
};

#include "yocto/container/slots.hpp"

//! base class for records
typedef dynamic_slots<Record> RecordsBase;

//! a set of Records
class Records : public RecordsBase
{
public:
    explicit Records(size_t M);
    virtual ~Records() throw();

    void Append( RArray<double> &data );
    
private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(Records);
};

#endif
