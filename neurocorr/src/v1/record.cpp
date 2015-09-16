#include "record.hpp"
#include "yocto/exception.hpp"

Record:: ~Record() throw()
{
}

static inline double *GetAddrOf(  RArray<double> &data )
{
    const size_t n = data.size();
    if(n<=0)
    {
        throw exception("Record: Empty Data!");
    }
    const size_t N = size_t(data[0]);
    if(N>n-1)
        throw exception("Record: Corrupted Data!");
    return N==0?NULL:&data[1];
}



static inline size_t GetSizeOf( RArray<double> &data)
{
    return (data.size()>0) ? size_t(data[0]) : 0;

}

Record:: Record(  RArray<double> &data ) :
LArray<double>( GetAddrOf(data), GetSizeOf(data) )
{
    
}


////////////////////////////////////////////////////////////////////////////////
Records:: ~Records() throw()
{

}

Records:: Records(size_t M) : RecordsBase(M)
{
}


void Records:: Append( RArray<double> &data)
{
    this->append<RArray<double>&>(data);
}