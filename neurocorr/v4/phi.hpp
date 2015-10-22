#ifndef NC_PHI_INCLUDED
#define NC_PHI_INCLUDED 1

#include "cpw.hpp"
#include "yocto/ptr/shared.hpp"

typedef vector<CPW> CPW_Functions;

class PHI_Functions : public CPW_Functions
{
public:
    const array<Unit> *train;
    explicit PHI_Functions(const array<Unit> &spikes, const size_t extra);
    explicit PHI_Functions() throw();
    virtual ~PHI_Functions() throw();

    //! build all function using delta
    void build(const Unit delta, UVector &shift);


private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(PHI_Functions);
};


#include "records.hpp"
typedef matrix<PHI_Functions> _PHI;



class PHI : public _PHI
{
public:
    explicit PHI(const Records &records,const size_t extra);
    virtual ~PHI() throw();

    const size_t K;         //!< 1+extra
    const size_t &trials;   //!< rows
    const size_t &neurones; //!< cols;
    const size_t  NK;       //!< neurones * K
    const size_t  dim;      //!< 1+NK
    const size_t  maxCount; //!< for memory
    mutable threading::sequential_executor kSeq;

    void build(const Unit deltaUnits, threading::crew *team );

    


private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(PHI);
    typedef shared_ptr<UVector> UVecPtr;
    Unit              delta;   //!< shared delta for threads
    threading::kernel kBuild;  //!< the kernel
    vector<UVecPtr>   shifts;  //!< local spaces
    void onBuild( threading::context &ctx );
    
};


#endif
