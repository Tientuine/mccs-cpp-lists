#include <algorithm>
#include <forward_list>
#include <iostream>
#include <iomanip>
#include <iterator>
#include <numeric>

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

    auto z = 0.f;
    for (auto j = 2.f; j <= m; ++j ) {
        forward_list<float> ys;
        transform(begin(xs), end(xs), front_inserter(ys), [j](int x)->float{return x/j;});
        ys.reverse();
        z += accumulate(begin(ys),end(ys),0.f);
    }
    std::cout << std::setprecision(12) << z << std::endl;

    //std::cout << drop( 100000, take( 100010, xs ) ) << std::endl;  
    //std::cout << drop( 100000, take( 100010, xs ) ) << std::endl;  

    //auto ps = filter( [](std::string const& s){return s[0] == 'M';}, ss );
    //std::cout << reverse(ps) << std::endl;  

    /*
    auto x = begin(xs);
    advance(x, 40);

    std::cout << '[' << *(x++);
    for (auto i = 1; i < 10; ++i, ++x) {
        std::cout << ',' << *x;
    }
    std::cout << ']' << std::endl;
    */

    return 0;
}

