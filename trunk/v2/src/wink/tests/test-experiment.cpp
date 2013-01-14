#include "../experiment.hpp"
#include <iostream>

using namespace wink;

int main(int argc, char *argv[] )
{
    try
    {
        experiment exp(5,100);
        for( size_t i=0; i < exp.trials; ++i )
        {
            record &rec = exp[i];
            std::cerr << "rec.size=" << rec.size << std::endl;
        }
        return 0;
    }
    catch(...)
    {
        std::cerr << "Unhandled Exception!" << std::endl;
    }
    return 1;
}