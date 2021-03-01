#include <iostream>
#include <iomanip>
#include "List.hpp"

using namespace prelude;

std::string::size_type len(std::string const& s) { return s.length(); }

int main(int argc, char** argv)
{
    auto n = std::stoi(argv[1]);
    auto m = std::stoi(argv[2]);

    list<int> xs = cons( n, empty<int>() );
    for (auto i = (n-1); i >= 1; --i) {
        xs = i | xs;
    }

    for (auto j = 2; j <= m; ++j ) {
        auto ys = filter( [j](int x){return x % j == 0;}, xs );
        std::cout << length( ys ) << std::endl;
    }

    return 0;
}

