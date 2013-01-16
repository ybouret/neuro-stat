#include "../../pyck/rand32.hpp"
#include "../../pyck/wtime.hpp"
#include "../../wink/drawing.hpp"

#include <iostream>

using namespace wink;

int main(int argc, char *argv[] )
{
    try
    {
    
        drawing drw;
        drw.reserve(1000);
        drw.identity(10);
        
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
