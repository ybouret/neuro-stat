#include "yocto/associative/map.hpp"
#include "yocto/mpa/rational.hpp"
#include "yocto/sequence/vector.hpp"
#include "yocto/R/R++.hpp"

using namespace yocto;

extern "C"
SEXP wink_xts( SEXP RCM ) throw()
{
    try
    {
        //______________________________________________________________________
        //
        // We have a R matrix=>convert it
        //______________________________________________________________________
        RMatrix<double> CM( RCM );
        if(CM.cols!=CM.rows)
        {
            throw exception("Coincidences Matrix is not square");
        }
        const size_t    M = CM.cols;
        
        //______________________________________________________________________
        //
        // collect all possible coincidences with their ocurences
        //______________________________________________________________________
        typedef map<size_t,size_t> dict_t;
        dict_t dict;
        
        for(size_t j=0;j<CM.cols;++j)
        {
            const RMatrix<double>::Column &col = CM[j];
            for(size_t i=0;i<CM.rows;++i)
            {
                if(i!=j)
                {
                    const size_t key = size_t(col[i]);
                    size_t *number = dict.search(key);
                    if(number)
                    {
                        assert( (*number)>0 );
                        ++(*number);
                    }
                    else
                    {
                        if(!dict.insert(key,1))
                        {
                            throw exception("unexpected dict insert failure");
                        }
                    }
                }
            }
        }
        const size_t nu = dict.size();
        Rprintf("#trials= %u\n", unsigned(M)  );
        Rprintf("#lambda= %u\n", unsigned(nu) );
        //______________________________________________________________________
        //
        // create the required lambda/ratio
        //______________________________________________________________________
        vector<size_t> lambda(nu,as_capacity);
        vector<mpq>    ratio(nu,as_capacity);
        {
            const mpn total = M*(M-1);
            for(dict_t::iterator i=dict.begin();i!=dict.end();++i)
            {
                const size_t k = i->key;
                lambda.push_back(k);
                const mpz     occurs = *i;
                const mpq     proba(occurs,total);
                ratio.push_back(proba);
            }
        }
        dict.release();
#if 1
        {
            mpq ratio_sum;
            for(size_t i=nu;i>0;--i)
            {
                ratio_sum += ratio[i];
            }
            if( ratio_sum.num != 1 || ratio_sum.den != 1)
                throw exception("Invalid Ratio Sum!");
            
        }
#endif
        std::cerr << "Lambda=" << lambda << std::endl;
        std::cerr << "ratio =" << ratio  << std::endl;

        
        
    }
    catch(const exception &e )
    {
        Rprintf("in wink_xts:\n%s\n%s\n",e.what(),e.when());
    }
    catch(...)
    {
        Rprintf("unhandled exception in wink_xts\n");
    }
    return R_NilValue;
}