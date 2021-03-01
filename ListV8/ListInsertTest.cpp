#include <algorithm>
#include <forward_list>
#include <iostream>
#include <iomanip>
#include <iterator>

std::string::size_type len(std::string const& s) { return s.length(); }

using namespace std;

int main(int argc, char** argv)
{
  auto n = stoi(argv[1]);
  auto m = stof(argv[2]) + 1.f;

  forward_list<int> xs;
  for (auto i = n; i >= 1; --i) {
    xs.push_front(i);
  }

  for (auto j = 0; j <= m; ++j ) {
    auto ys(xs);
    auto y = begin(ys);
    advance(y, 49);
    ys.insert_after(y, j);
  }

  auto x = begin(xs);
  advance(x, 50);

  std::cout << *x << std::endl;

  return 0;
}

