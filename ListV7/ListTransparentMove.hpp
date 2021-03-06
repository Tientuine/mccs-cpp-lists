#ifndef HPP_PRELUDE_LIST
#define HPP_PRELUDE_LIST

#include <memory>
#include <functional>

/**
 * Implementation of Haskell-like, functional programming idioms, particularly recursive Lists.
 */
namespace prelude {

template <typename A> class Node;

/**
 * Family of recursively-defined homogeneous list types.
 * Lists create via constructor or the cons operator (overloaded |) use
 * shallow copy for the tail, which helps ease the storage burden for
 * memory-heavy element types.
 * 
 * @author Matthew A Johnson
 * @version 1.0
 */
template <typename A>
using List = std::shared_ptr<Node<A>>;

/**
* Internal node structure for a list.
* @note required so that List can provide a distinct empty-list value
*/
template <typename B>
class Node
{
  friend List<B>;
public:
  /**
   * Make an internal list node from an element and a pointer to a node.
   * @param x  an element
   * @param xs pointer to an existing node (or nullptr)
   */
  Node(B x, std::shared_ptr<Node> xs) : _datum(x), _tail(std::move(xs)) {}

private:
  /** The value of an element. */
  B _datum;
  /** Node containing the next element. */
  List<B> _tail;

  template <typename... Args> friend std::shared_ptr<Node> std::make_shared (Args&&...);
  template <typename A> friend bool    null( List<A> const& );
  template <typename A> friend A       head( List<A> const& );
  template <typename A> friend List<A> tail( List<A> const& );
  template <typename A> friend List<A> operator| ( A x, List<A> );
};

/** Typed empty list constant - could implement as a method instead. */
template <typename A>
List<A> EMPTY { nullptr };

/**
 * Constructs a list by pre-pending an element to an existing list.
 * @param x an element
 * @param xs a list
 * @return a list with <var>x</var> at the head and <var>xs</var> as the tail
 */
template <typename A>
inline List<A> operator| ( A x, List<A> xs ) { return std::move( std::make_shared<Node<A>>( x, std::move(xs) ) ); }

// List operations

/**
 * Obtains a new list by applying a function to each element of a list.
 * Converts a list of one type to that of another by applying a specified function.
 * @param f function that takes an element of <var>xs</var> and returns a different type
 * @param xs a list
 * @return a list of elements the same type as the return type of <var>f</var>
 */
template <typename F, typename A>
inline auto map( F f, List<A> const& xs ) -> List<decltype(f(head(xs)))>
{
  if ( null( xs ) ) { return EMPTY<decltype(f(head(xs)))>; }
  return std::move( f( head( xs ) ) | std::move( map<F,A>( f, std::move( tail( xs ) ) ) ) );
}

/**
 * Appends one list to another.
 * @param xs a list
 * @param ys a list
 * @return a list in which the elements of <var>xs</var> precede the elements of <var>ys</var>
 * @note element order is preserved
 */
template <typename A>
inline List<A> operator+ ( List<A> const& xs, List<A> ys )
{
  if ( null( xs ) ) { return std::move(ys); }
  return std::move( List<A>( head( xs ), std::move( tail( xs ) ) + std::move(ys) ) );
}

/**
 * Extracts a sublist of those elements satisfying the given predicate.
 * @param pred a predicate function
 * @param xs a list
 * @return a list containing those elements of <var>xs</var> satisfying <var>pred</var>
 */
template <typename P, typename A>
inline List<A> filter( P pred, List<A> const& xs )
{
  if ( null( xs ) ) { return EMPTY<A>; }
  auto ys = std::move( filter( pred, std::move( tail( xs ) ) ) );
  auto x = head( xs );
  return pred( x ) ? (x | ys) : std::move(ys);
}

/**
 * Extracts the first element of a list.
 * @param xs a non-empty list
 * @return the first element of <var>xs</var>
 */
template <typename A>
inline A head( List<A> const& xs )
{
  if ( null( xs ) ) { throw xs; }
  return xs->_datum;
}

/**
 * Extracts the last element of a list.
 * @param xs a non-empty list
 * @return the last element of <var>xs</var>
 */
template <typename A>
inline A last( List<A> const& xs )
{
  if ( null( xs ) ) { throw xs; }

  auto ys = tail( xs );
  if ( null( ys ) ) {
    return head( xs );
  } else {
    return last( ys );
  }
}

/**
 * Extracts all elements after the head of a list.
 * @param xs a non-empty list
 * @return a list containing all but the first element of <var>xs</var>
 */
template <typename A>
inline List<A> tail( List<A> const& xs )
{
  if ( null( xs ) ) { throw xs; }
  return std::move(xs->_tail);
}

/**
 * Extracts all except the last element of a list.
 * @param xs a non-empty list
 * @return a list containing all but the last element of <var>xs</var>
 */
template <typename A>
inline List<A> init( List<A> const& xs )
{
  if ( null( xs ) ) { throw xs; }

  auto ys = tail( xs );
  if ( null( ys ) ) {
    return std::move( ys );
  } else {
    return std::move( head( xs ) | init( ys ) );
  }
}

/**
 * Tests whether a list is empty.
 * @param xs a list
 * @return true if <var>xs</var> is empty, false otherwise
 */
template <typename A>
inline bool null( List<A> const& xs ) { return !xs; }

/**
 * Computes the length of a list.
 * @param xs a list
 * @return the number of elements in <var>xs</var>
 * @note This operation has time complexity O(n).
 */
template <typename A>
inline unsigned length( List<A> const& xs )
{
  if ( null( xs ) ) {
    return 0;
  } else {
    return 1 + length( tail( xs ) );
  }
}

/**
 * Reverses a list.
 * @param xs a list
 * @return a list whose elements are the same as <var>xs</var> but in reverse order
 */
template <typename A>
inline List<A> reverse( List<A> const& xs )
{
  std::function<List<A>(List<A>, List<A>)> rev;
  rev = [&rev](List<A> xs, List<A> ys)->List<A>{
    if ( null( xs ) ) { return std::move( ys ); }
    return rev( tail( xs ), std::move( head( xs ) | ys ) );
  };
  return std::move( rev( xs, EMPTY<A> ) );
}

// Special folds

/**
 * Extracts a sublist of those elements satisfying the given predicate.
 * @param pred a predicate function
 * @param xs a list
 * @return a list containing those elements of <var>xs</var> satisfying <var>pred</var>
 */
template <typename A>
inline A sum( List<A> const& xs )
{
  if ( null( xs ) ) { return 0; }
  return head( xs ) + sum( tail( xs ) );
}

// Sublists

/**
 * Gets from a list its leading sublist of a given size if one exists.
 * @param k a non-negative integer
 * @param xs a list
 * @return a sublist of <var>xs</var> with at most <var>k</var> elements
 * @note returns an empty list if <var>xs</var> is empty
 */
template <typename A>
inline List<A> take( unsigned k, List<A> const& xs )
{
  if ( k == 0 || null( xs ) ) { return EMPTY<A>; }

  return std::move( head( xs ) | take( k - 1, tail( xs ) ) );
}

/**
 * Gets what remains after removing a given number of elements from a list.
 * @param k a non-negative integer
 * @param xs a list
 * @return a sublist of <var>xs</var> with at most <var>length-k</var> elements
 * @note returns an empty list if <var>xs</var> has fewer than <var>k</var> elements
 */
template <typename A>
inline List<A> drop( unsigned k, List<A> const& xs )
{
  if ( k == 0 || null( xs ) ) { return std::move( xs ); }

  return std::move( drop( k - 1, tail( xs ) ) );
}

// Converting to and from strings

/**
 * Inserts a character string serialization of a list into an output stream.
 * @param os an output stream
 * @param xs a list
 * @return a reference to the output stream
 */
template <typename A>
std::ostream& operator<< ( std::ostream& os, List<A> const& xs )
{
  auto n = xs;
  if ( n ) {
    os << '[' << n->_datum;
    while ( (n = n->_tail) ) {
      os << ',' << n->_datum;
    }
    os << ']';
  }
  return os;
}


} // end namespace prelude

#endif //HPP_PRELUDE_LIST
