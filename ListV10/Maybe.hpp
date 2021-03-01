#ifndef HPP_MAYBE_LIST
#define HPP_MAYBE_LIST

#include <stdexcept>
#include <optional>

/**
 * Implementation of Haskell-like, functional programming idioms, particularly recursive lists.
 */
namespace prelude {

    template <typename A>
    using maybe = std::optional<A> const;

    template <typename A>
    maybe<A> just( A x ) { return { x }; }

    template <typename A>
    maybe<A> nothing() { return {}; }

} // end namespace prelude

#endif //HPP_MAYBE_LIST

