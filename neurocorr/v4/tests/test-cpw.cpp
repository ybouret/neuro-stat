#include "../cpw.hpp"
#include "yocto/utest/run.hpp"


static inline void check_intg(const CPW &F)
{
    if(F.size()>0)
    {
        const Unit a = F.front().tau - 1;
        const Unit b = F.back().tau  + 1;
        if( F.integrate(a,b) != F.integrate_(a,b) )
        {
            throw exception("check intg mismatch");
        }
    }
}

YOCTO_UNIT_TEST_IMPL(cpw)
{
    CPW F;
    std::cerr << "f0=" << F << std::endl;
    F.save("f0.dat");
    F.save_sample("f0v.dat");

    F.add(0,1);
    std::cerr << "f1=" << F << std::endl;
    F.save("f1.dat");
    F.save_sample("f1v.dat");
    check_intg(F);

    F.add(-1,2);
    std::cerr << "f2=" << F << std::endl;
    F.save("f2.dat");
    F.save_sample("f2v.dat");
    check_intg(F);

    Train train;
    vector<Unit> shift;
    F.buildFrom(train, 5,shift);
    F.save("phi0.dat");
    F.save_sample("phi0v.dat");
    check_intg(F);

    train.push_back(0);
    F.buildFrom(train, 5, shift);
    F.save("phi1.dat");
    F.save_sample("phi1v.dat");
    check_intg(F);

    train.push_back(1);
    F.buildFrom(train,5,shift);
    F.save("phi2.dat");
    F.save_sample("phi2v.dat");
    check_intg(F);

    train.push_back(5);
    F.buildFrom(train,5,shift);
    F.save("phi3.dat");
    F.save_sample("phi3v.dat");
    check_intg(F);
}
YOCTO_UNIT_TEST_DONE()
