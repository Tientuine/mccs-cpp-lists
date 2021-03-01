#ifndef HPP_PRELUDE_LIST
#define HPP_PRELUDE_LIST

#include <tuple>
#include <memory>

/**
 * Implementation of Haskell-like, functional programming idioms, particularly recursive Lists.
 */
namespace prelude {

template <typename T>
using List = std::pair<T,std::shared_ptr<List<T>>;

}

