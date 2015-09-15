#include "record.hpp"
#include "yocto/exception.hpp"

Record:: ~Record() throw()
{
}

static inline double *GetAddrOf(  array<double> &data )
{
    const size_t n = data.size();
    if(n<=0)
    {
        throw exception("Record: Empty Data!");
    }
    const size_t N = size_t(data[1]);
    if(N>n-1)
        throw exception("Record: Corrupted Data!");
    return N==0?NULL:&data[2];
}



static inline size_t GetSizeOf( array<double> &data)
{
    return (data.size()>0) ? size_t(data[1]) : 0;

}

Record:: Record(  array<double> &data ) :
lw_array<double>( GetAddrOf(data), GetSizeOf(data) )
{
    
}


////////////////////////////////////////////////////////////////////////////////
Records:: ~Records() throw()
{

}

Records:: Records(size_t M) : slots_of<Record>(M)
{
}


void Records:: Append(array<double> &data)
{
    this->append<array<double>&>(data);
}