#include <iostream>
#include <iomanip>
#include "List.hpp"

using namespace prelude;

std::string::size_type len(std::string const& s) { return s.length(); }

int main(int argc, char** argv)
{
    std::cout << "List<int> requires " << sizeof(List<int>) << std::endl;
    std::cout << "Node<int> requires " << sizeof(List<int>::Node) << std::endl;

    auto n = std::stoi(argv[1]);
    auto m = std::stof(argv[2]) + 1.f;

    List<int> xs { n, List<int>::EMPTY };
    for (auto i = (n-1); i >= 1; --i) {
        xs = i | std::move(xs);
    }

    auto z = 0.f;
    for (auto j = 2.f; j <= m; ++j ) {
        auto ys = map( [j](int x)->float{return x/j;}, xs );
        z += sum( ys );
    }

    std::cout << std::setprecision(12) << z << std::endl;

    //std::cout << drop( 40, take( 50, xs ) ) << std::endl;  

    //std::cout << drop( 100000, take( 100010, xs ) ) << std::endl;  

    //auto ps = filter( [](std::string const& s){return s[0] == 'M';}, ss );
    //std::cout << reverse(ps) << std::endl;  

    return 0;
}

