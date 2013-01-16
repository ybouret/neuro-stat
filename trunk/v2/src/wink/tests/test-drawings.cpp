#include "../../pyck/rand32.hpp"
#include "../../pyck/wtime.hpp"
#include "../../wink/drawing.hpp"

#include <iostream>

using namespace wink;

int main(int argc, char *argv[] )
{
    try
    {

        DefaultUniformGenerator ran;
        ran.seed( WallTime::Seed() );
        drawing drw;
        drw.reserve(1000);
        drw.identity(10);
        std::cerr << "drw=" << drw << std::endl;
        for( size_t i=0; i < 10; ++i )
        {
            drw.permutation(10,ran);
            std::cerr << "drw=" << drw << std::endl;
        }
        return 0;
    }
    catch( const std::exception &e )
    {
        std::cerr << "*** Exception: " << e.what() << std::endl;
    }
    catch(...)
    {
        std::cerr << "Unhandled Exception!" << std::endl;
    }
    return 1;
}
