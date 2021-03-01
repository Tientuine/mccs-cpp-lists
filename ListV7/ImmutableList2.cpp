#include <cstdlib>
#include <iostream>
#include "ImmutableList2.hpp"

int main(int argc, char** argv)
{
    auto const M = std::atoi(argv[1]);
    auto const N = std::atoi(argv[2]);

    for (auto k = 0; k < N; ++k) {
        List<int> xs { EMPTY<int> };
        for (auto x = 0; x < N; ++x) {
            xs = x | xs;
        }
        for (auto i = 0; i < N; ++i) {
            xs = tail(xs);
        }
    }

    return 0;
}

