#include "Maybe.hpp"

int main(int argc, char** argv)
{
    maybe<int> x = just(5);
    maybe<int> y = nothing();

    return 0;
}

