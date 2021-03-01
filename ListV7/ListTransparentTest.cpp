#include <iostream>
#include <iomanip>
#include "ListTransparent.hpp"

using namespace prelude;

std::string::size_type len(std::string const& s) { return s.length(); }

int main(int argc, char** argv)
{
  auto n = std::stoi(argv[1]);
  auto m = std::stof(argv[2]) + 1.f;

  List<int> xs = n | EMPTY<int>;
  for (auto i = (n-1); i >= 1; --i) {
    xs = i | xs;
  }

  auto z = 0.f;
  for (auto j = 2.f; j <= m; ++j ) {
    auto ys = map( [j](int x)->float{return x/j;}, xs );
    //std::cout << ys << std::endl;
    z += sum( ys );
  }
  std::cout << std::setprecision(8) << z << std::endl;

  //std::cout << drop( 100000, take( 100010, xs ) ) << std::endl;  
  //std::cout << drop( 100000, take( 100010, xs ) ) << std::endl;  

  //auto ps = filter( [](std::string const& s){return s[0] == 'M';}, ss );
  //std::cout << reverse(ps) << std::endl;  

  return 0;
}

/*
#include <iostream>
#include "List.hpp"

using namespace prelude;

std::string::size_type len(std::string const& s) { return s.length(); }

int main(int argc, char** argv)
{
  List<int> xs(5, List<int>::EMPTY);
  
  List<int> ys(8, xs);

  std::cout << head(tail(ys)) << std::endl;
  
  List<int> zs {10, 8, 5, 2, 4, 1, 0};

  std::cout << head(tail(zs)) << std::endl;

  List<int> ws(1, List<int>::EMPTY);
  for (auto i = 2; i <= 200000; ++i) {
    ws = i | ws;
  }
  std::cout << ws[0] << ' ' << take(5,tail(ws)) << ' ' << drop(length(ws)-5,ws) << std::endl;  

  List<std::string> ss { "Marist", "MCCS", "C++", "Haskell" };

  auto ls = map( std::bind(&std::string::size, std::placeholders::_1), ss );
  auto ks = map( &len, ss );
  auto js = map( [](std::string const& s){return s.length();}, ss );

  std::cout << ls << std::endl;  
  std::cout << ss << std::endl;  

  auto ps = filter( [](std::string const& s){return s[0] == 'M';}, ss );

  std::cout << reverse(ps) << std::endl;  

  return 0;
}
*/
