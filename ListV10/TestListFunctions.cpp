#include <cassert>
#include <cmath>
#include <functional>
#include <iostream>
#include <sstream>

#include "List.hpp"

using namespace prelude;

template <typename F, typename A, typename B>
void test( F f, list<A> xs, B x )
{
    assert( f( xs ) == x );
}

template <typename F, typename A>
void test( F f, list<A> xs, list<A> ys )
{
    assert( f( xs ) == ys );
}

template <typename F, typename A>
void test( F f, list<A> const& xs, list<A> const& ys, list<A> const& zs )
{
    //std::cerr << f( xs, ys ) << std::endl;
    assert( f( xs, ys ) == zs );
}

int main()
{
    using namespace std::placeholders;

    std::ostringstream oss;
    list<double> xs1 { 1.0 };
    list<double> xs  { 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0 };

    test( null<double>, empty<double>(), true );
    test( null<double>, xs1, false );
    test( null<double>, xs,  false );

    test( length<double>, empty<double>(), 0 );
    test( length<double>, xs1, 1 );
    test( length<double>, xs,  9 );

    test( head<double>, xs1, 1.0 );
    test( head<double>, xs,  2.0 );
    test( last<double>, xs1, 1.0 );
    test( last<double>, xs, 10.0 );
    test( tail<double>, xs1, empty<double>() );
    test( tail<double>, xs, { 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0 } );
    test( init<double>, xs1, empty<double>() );
    test( init<double>, xs, { 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0 } );

    test( std::bind(cons<double>, 0.0, _1), empty<double>(), list<double>{ 0.0 } );
    test( std::bind(cons<double>, 0.0, _1), xs1, { 0.0, 1.0 } );
    test( std::bind(cons<double>, 1.0, _1), xs,  { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0 } );

    test( std::bind(take<double>, 3, _1), empty<double>(), empty<double>() );
    test( std::bind(take<double>, 3, _1), xs1, xs1 );
    test( std::bind(take<double>, 3, _1), xs,  { 2.0, 3.0, 4.0 } );

    test( std::bind(drop<double>, 3, _1), empty<double>(),  empty<double>() );
    test( std::bind(drop<double>, 3, _1), xs1, empty<double>() );
    test( std::bind(drop<double>, 3, _1), xs,  { 5.0, 6.0, 7.0, 8.0, 9.0, 10.0 } );

    test( append<double>, empty<double>(), empty<double>(), empty<double>() );
    test( append<double>, empty<double>(), xs1, xs1 );
    test( append<double>, empty<double>(), xs,  xs  );
    test( append<double>, xs1, empty<double>(), xs1 );
    test( append<double>, xs1, xs1, { 1.0, 1.0 } );
    test( append<double>, xs1, xs,  { 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0 } );
    test( append<double>, xs, empty<double>(), xs );
    test( append<double>, xs, xs1, { 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 1.0 } );
    test( append<double>, xs, xs, { 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0 } );

    test( reverse<double>, empty<double>(), empty<double>() );
    test( reverse<double>, xs1, xs1 );
    test( reverse<double>, xs, { 10.0, 9.0, 8.0, 7.0, 6.0, 5.0, 4.0, 3.0, 2.0 } );

    test( sum<double>, empty<double>(), 0.0 );
    test( sum<double>, xs1, 1.0 );
    test( sum<double>, xs, 54.0 );

    auto f1 = [](list<double> const& ys){ return filter([](double x){return std::fmod(x,2) == 0;}, ys); };
    auto f2 = [](list<double> const& ys){ return filter([](double x){return std::fmod(x,2) == 1;}, ys); };
    
    test( f1, empty<double>(), empty<double>() );
    test( f1, xs1, empty<double>() );
    test( f2, xs1, { 1.0 } );
    test( f1, xs, { 2.0, 4.0, 6.0, 8.0, 10.0 } );
    test( f2, xs, { 3.0, 5.0, 7.0, 9.0 } );

    auto g = [&oss](list<double> const& ys){ oss.str(""); oss << ys; return oss.str(); };
    test( g, empty<double>(), "[]" );
    test( g, xs1, "[1]" );
    test( g, xs,  "[2,3,4,5,6,7,8,9,10]" );

    return 0;
}

