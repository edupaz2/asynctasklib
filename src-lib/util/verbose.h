#ifndef _VERBOSE_H_
#define _VERBOSE_H_

class Verbose {
public:
    Verbose( std::string const& d, std::string const& s="stop");
    ~Verbose();

    Verbose( Verbose const&) = delete;
    Verbose & operator=( Verbose const&) = delete;

private:
    std::string     desc;
    std::string     stop;
};
#endif