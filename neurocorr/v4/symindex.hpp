#ifndef SYMINDEX_INCLUDED
#define SYMINDEX_INCLUDED 1


#include "yocto/os.hpp"

class SymIndex
{
public:
    inline SymIndex(const size_t usrN):
    N(usrN),
    items((N*(N+1))>>1),
    QI( new size_t[N] )
    {
        assert(N>0);
        const size_t NN=(N<<1)+1;
        for(size_t I=0;I<N;++I)
        {
            QI[I] = (I*(NN-I))>>1;
            assert( (*this)(I,I) == QI[I] );
        }
    }

    inline ~SymIndex() throw()
    {
        delete []QI;
    }


    const size_t N;
    const size_t items;

    inline size_t operator()(const size_t I, const size_t J) const throw()
    {
        assert(I<N);
        assert(J>=I);
        assert(J<N);
        return (N*I) + J - ((I*(I+1))>>1);
    }

    inline size_t operator()(const size_t I) const throw()
    {
        assert(I<N);
        return QI[I];
    }

    inline void FindIJ(const size_t Q,
                       size_t      &I,
                       size_t      &J) const throw()
    {
        assert(Q<items);
        size_t        ilo = 0;
        size_t        iup = N;
        const size_t *QII = QI;
        const size_t  Qup = QII[--iup];
        if(Q>=Qup)
        {
            I = iup;
            J = I;
        }
        else
        {
            while(iup-ilo>1)
            {
                const size_t imid = (ilo+iup)>>1; assert(imid<N);
                const size_t qmid = QII[imid];
                if(qmid<=Q)
                {
                    ilo = imid;
                }
                else
                {
                    iup = imid;
                }
            }
            I = ilo;
            J = I + (Q-QII[ilo]);
        }
        assert( Q == (*this)(I,J) );
    }
    
    
    
private:
    YOCTO_DISABLE_COPY_AND_ASSIGN(SymIndex);
    size_t *QI;
};

#endif
