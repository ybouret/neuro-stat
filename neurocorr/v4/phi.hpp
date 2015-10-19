#ifndef NC_PHI_INCLUDED
#define NC_PHI_INCLUDED 1

#include "cpw.hpp"

typedef vector<CPW> CPW_Functions;

class PHI_Functions : public CPW_Functions
{
public:
    const array<Unit> &train;
    explicit PHI_Functions(const array<Unit> &spikes, const size_t extra);
    virtual ~PHI_Functions() throw();

    //! build all function using delta
    void build(const Unit delta);


private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(PHI_Functions);
};

#include "records.hpp"

typedef matrix<PHI_Functions> _PHI;

class PHI : public _PHI
{
public:
    explicit PHI(const Records &records);
    virtual ~PHI() throw();

private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(PHI);
};


#endif
