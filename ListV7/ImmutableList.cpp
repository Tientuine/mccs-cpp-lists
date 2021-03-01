#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>

template <typename T> class ListBase;
template <typename T> class EmptyList;
template <typename T> class NonEmptyList;

template <typename T>
using List = std::shared_ptr<ListBase<T> const>;

template <typename T>
class ListBase {
public:
    virtual T const& head() const = 0; // pure virtual - must be defined in the subclass
    virtual List<T>  tail() const = 0;
};

template <typename T>
class EmptyList : public ListBase<T>
{
public:
    // singleton design pattern - only one instance is allowed
    static EmptyList const EMPTY;
    virtual T const& head() const { throw std::exception(); }
    virtual List<T>  tail() const { throw std::exception(); }
private:
    explicit EmptyList() {}
};

template<typename T>
EmptyList<T> const EmptyList<T>::EMPTY {};

template <typename T>
List<T> EMPTY() { return std::make_shared<EmptyList<T> const>(EmptyList<T>::EMPTY); }

template <typename T>
class NonEmptyList : public ListBase<T>
{
public:
    NonEmptyList(T x, List<T> const& xs) : _head(x), _tail(xs) {}
    NonEmptyList(T x, List<T> && xs) : _head(x), _tail(std::forward<List<T>>(xs)) {}
    NonEmptyList(T x) : NonEmptyList(x, EMPTY<T>()) {}

    virtual T const& head() const { return  _head; }
    virtual List<T>  tail() const { return _tail; }
private:
    T       _head;
    List<T> _tail;
};

template <typename T>
List<T> operator| (T x, List<T> const& xs) { return List<T>(new NonEmptyList<T>(x, xs)); }

template <typename T>
List<T> operator| (T x, List<T> && xs) { return List<T>(new NonEmptyList<T>(x, std::forward<List<T>>(xs))); }

int main(int argc, char** argv)
{
    auto const M = std::atoi(argv[1]);
    auto const N = std::atoi(argv[2]);

    for (auto k = 0; k < N; ++k) {
        List<int> xs { EMPTY<int>() };
        for (auto x = 0; x < N; ++x) {
            xs = x | xs;
        }
        for (auto i = 0; i < N; ++i) {
            xs = xs->tail();
        }
    }

    return 0;
}

