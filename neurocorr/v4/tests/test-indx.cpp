#include "../types.hpp"
#include "../symindex.hpp"
#include "yocto/utest/run.hpp"

#if 0
static inline
size_t getQ(const size_t I,const size_t J,const size_t NK)
{
    size_t Q = 0;
    for(size_t i=0;i<I;++i)
    {
        Q += (NK-i);
    }
    Q += (J-I);
    return Q;
}

static inline
size_t getQ2(const size_t I,const size_t J,const size_t NK)
{
    return I*NK+J-(I*(I+1))/2;
}
#endif


YOCTO_UNIT_TEST_IMPL(indx)
{
    for(size_t NK=1;NK<=200;++NK)
    {
        SymIndex     S(NK);
        const size_t nmax = (NK*(NK+1))/2;
        std::cerr << "NK=" << NK << "=>" <<  nmax << std::endl;
        for(size_t I=0;I<NK;++I)
        {
            if(S(I,I)!=S(I)) throw exception("Mismatch S(%u)=%u and S(%u,%u)=%u", unsigned(I), unsigned(S(I)), unsigned(I), unsigned(I), unsigned(S(I,I)) );
        }
        size_t q = 0;
        for(size_t I=0;I<NK;++I)
        {
            for(size_t J=I;J<NK;++J)
            {
                //const size_t Q  = getQ(I,J,NK);
                //const size_t Q2 = getQ2(I,J,NK);
                //std::cerr << "I=" << I << ", J=" << J << " => Q=" <<  Q << std::endl;
                const size_t Q = S(I,J);
                if(q!=Q)
                    throw exception("Mismatch indices for (%u,%u)", unsigned(I), unsigned(J) );
                size_t Iguess=0,Jguess=0;
                S.FindIJ(Q,Iguess,Jguess);
                if(Iguess!=I)
                {
                    throw exception("Mismatch Iguess=%u/I=%u\n", unsigned(Iguess), unsigned(I) );
                }
                if(Jguess!=J)
                {
                    throw exception("Mismatch Jguess=%u/J=%u\n", unsigned(Jguess), unsigned(J) );
                }
                
                ++q;
            }
        }
        assert(q==nmax);

    }
}
YOCTO_UNIT_TEST_DONE()
