#include <iostream>
#include <iomanip>
#include "List.hpp"

using namespace prelude;

std::string::size_type len(std::string const& s) { return s.length(); }

int main(int argc, char** argv)
{
  auto n = std::stoi(argv[1]);
  auto m = std::stoi(argv[2]);

  List<int> xs { n, List<int>::EMPTY };
  for (auto i = (n-1); i >= 1; --i) {
    xs = i | std::move(xs);
  }

  for (auto j = 0; j < m; ++j ) {
    auto ys = take( 50, xs ) + (j | drop( 50, xs ));
  }

  std::cout << xs[50] << std::endl;

  return 0;
}

