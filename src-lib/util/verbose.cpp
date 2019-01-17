#include "verbose.h"

Verbose::Verbose( std::string const& d, std::string const& s="stop") :
    desc( d),
    stop( s) {
    std::cout << desc << " start" << std::endl;
}

Verbose::~Verbose() {
    std::cout << desc << ' ' << stop << std::endl;
}