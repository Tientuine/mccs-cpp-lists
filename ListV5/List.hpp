#ifndef HPP_PRELUDE_LIST
#define HPP_PRELUDE_LIST

#include <memory>
#include <functional>

/**
 * Implementation of Haskell-like, functional programming idioms, particularly recursive Lists.
 */
namespace prelude {



/**
 * Family of recursively-defined homogeneous list types.
 * Lists create via constructor or the cons operator (overloaded |) use
 * shallow copy for the tail, which helps ease the storage burden for
 * memory-heavy element types.
 * 
 * @author Matthew A Johnson
 * @version 1.0
 */
template <typename T>
class List
{
/**
 * Internal node structure for a list.
 * @note required so that List can provide a distinct empty-list value
 */

struct Node
{
    using Ptr = std::shared_ptr<Node>;

    /**
     * Make an internal list node from an element and a pointer to a node.
     * @param x  an element
     * @param xs pointer to an existing node (or nullptr)
     */
    Node( T const& x, Ptr const& xs = EMPTY ) : _datum(x), _tail(xs) {}
    Node( T const& x, Ptr &&     xs ) : _datum(x), _tail(std::forward<Ptr>(xs)) {}
    Node( T && x, Ptr const& xs = nullptr ) : _datum(std::forward<T>(x)), _tail(xs) {}
    Node( T && x, Ptr &&     xs ) : _datum(std::forward<T>(x)), _tail(std::forward<Ptr>(xs)) {}

    /** The value of an element. */
    T _datum;
    /** Node containing the next element. */
    Ptr _tail;
};

public:
  /** Typed empty list constant - could implement as a method instead. */
  static List const EMPTY;

  /**
   * Make a list by prepending an element to an existing list.
   * @param x  the head of the new list
   * @param xs the tail of the new list, {@link EMPTY} if not provided
   * @note this constructor will never be called implicitly
   */
  explicit List( T const& x, List const& xs = EMPTY )
    : _head( std::make_shared<Node>( x, xs._head ) ) {}

  /**
   * Make a list using standard universal initializer.
   */
  List( std::initializer_list<T> xs ) {
    for (auto i = xs.end(); i-- != xs.begin();) {
      _head = std::make_shared<Node>(*i, _head);
    }
  }

  /**
   * Element access by zero-based index.
   * @param position of the desired element
   * @return an element of the list
   * @note throws an exception for out-of-range index
   */
  T operator[] ( unsigned k ) const {
    if ( !_head ) { throw EMPTY; }
    if ( k == 0 ) {
      return head( *this );
    } else {
      return tail( *this )[ k-1 ];
    }
  }

private:
  // pointer to internal node represenation
  std::shared_ptr<Node> _head;

  // Permits implicit construction from pointer-to-Node (used internally & by friends)
  List( std::shared_ptr<Node> xs = nullptr ) : _head( xs ) {}

  // Various friends allow for non-OOP functional-style

  template <typename F,typename U> friend auto map( F f, List<U> const& xs ) -> List<decltype(f(head(xs)))>;
  template <typename U> friend U sum( List<U> const& xs );
  template <typename P, typename U> friend List<U> filter( P, List<U> const& );
  template <typename U> friend bool null( List<U> const& );
  template <typename U> friend unsigned length( List<U> const& );
  template <typename U> friend U       head( List<U> const& );
  template <typename U> friend List<U> tail( List<U> const& );
  template <typename U> friend U       last( List<U> const& );
  template <typename U> friend List<U> init( List<U> const& );
  template <typename U> friend List<U> take( unsigned, List<U> const& );
  template <typename U> friend List<U> drop( unsigned, List<U> const& );
  template <typename U> friend List<U> reverse( List<U> const& );
  template <typename U> friend List<U> operator| ( U, List<U> const& );
  template <typename U> friend List<U> operator| ( U, List<U> && );
  template <typename U> friend List<U> operator+ ( List<U> const&, List<U> const& );
  template <typename U> friend std::ostream& operator<< ( std::ostream&, List<U> const& );
};

template <typename T> List<T> const List<T>::EMPTY;
template <typename T> List<T> const EMPTY() { return List<T>::EMPTY; }

// Friend implementations below

/**
 * Constructs a list by pre-pending an element to an existing list.
 * @param x an element
 * @param xs a list
 * @return a list with <var>x</var> at the head and <var>xs</var> as the tail
 */
template <typename U>
inline List<U> operator| ( U x, List<U> const& xs ) { return List<U>( x, xs ); }
template <typename U>
inline List<U> operator| ( U x, List<U> && xs ) { return List<U>( std::forward(x), std::forward(xs) ); }

// List operations

/**
 * Obtains a new list by applying a function to each element of a list.
 * Converts a list of one type to that of another by applying a specified function.
 * @param f function that takes an element of <var>xs</var> and returns a different type
 * @param xs a list
 * @return a list of elements the same type as the return type of <var>f</var>
 */
template <typename F, typename U>
inline auto map( F f, List<U> const& xs ) -> List<decltype(f(head(xs)))>
{
    using V = decltype(f(head(xs)));
    using Node = typename List<V>::Node;

    if ( null( xs ) ) { return List<V>::EMPTY; }

    auto ys = List<V>( f( head( xs ) ) );
    auto from = xs._head;
    auto to = ys._head;
    while ( (from = from->_tail) ) {
        to = to->_tail = std::make_shared<Node>( f( from->_datum ) );
    }
    return ys;
}

/**
 * Extracts a sublist of those elements satisfying the given predicate.
 * @param pred a predicate function
 * @param xs a list
 * @return a list containing those elements of <var>xs</var> satisfying <var>pred</var>
 */
template <typename U>
U sum( List<U> const& xs )
{
  U n = 0;
  auto e = xs._head;
  while (e) {
    n += e->_datum;
    e = e->_tail;
  }
  return n;
}

/**
 * Extracts a sublist of those elements satisfying the given predicate.
 * @param pred a predicate function
 * @param xs a list
 * @return a list containing those elements of <var>xs</var> satisfying <var>pred</var>
 */
template <typename P, typename U>
List<U> filter( P pred, List<U> const& xs )
{
  using Node = typename List<U>::Node;

  List<U> ys;
  auto from = xs._head, to = ys._head;
  while ( from ) {
    if ( pred( from->_datum ) ) {
      to = to->_tail = std::make_shared<Node>( from->_datum );
    }
    from = from->_tail;
  }
  return ys;
}

/**
 * Tests whether a list is empty.
 * @param xs a list
 * @return true if <var>xs</var> is empty, false otherwise
 */
template <typename U>
bool null( List<U> const& xs ) { return !xs._head; }

/**
 * Computes the length of a list.
 * @param xs a list
 * @return the number of elements in <var>xs</var>
 * @note This operation has time complexity O(n).
 */
template <typename U>
unsigned length( List<U> const& xs )
{
  auto n = 0;
  auto e = xs._head;
  while ( e ) { e = e->_tail; ++n; }
  return n;
}

/**
 * Extracts the first element of a list.
 * @param xs a non-empty list
 * @return the first element of <var>xs</var>
 */
template <typename U>
U head( List<U> const& xs )
{
  if ( null( xs ) ) { throw xs;}
  return xs._head->_datum;
}

/**
 * Extracts all elements after the head of a list.
 * @param xs a non-empty list
 * @return a list containing all but the first element of <var>xs</var>
 */
template <typename U>
List<U> tail( List<U> const& xs )
{
  if ( null( xs ) ) { throw xs; }
  return xs._head->_tail;
}

/**
 * Extracts all except the last element of a list.
 * @param xs a non-empty list
 * @return a list containing all but the last element of <var>xs</var>
 */
template <typename U>
List<U> init( List<U> const& xs )
{
  using Node = typename List<U>::Node;

  auto  from = xs._head;
  if ( !from ) { throw xs; }

  List<U> ys( from->_datum );

  auto to = ys._head;
  while ( (from = from->_tail) ) {
    to = to->_tail = std::make_shared<Node>( from->_datum );
  }
  return ys;
}

/**
 * Extracts the last element of a list.
 * @param xs a non-empty list
 * @return the last element of <var>xs</var>
 */
template <typename U>
U last( List<U> const& xs )
{
  if ( null( xs ) ) { throw List<U>::EMPTY; }
  auto e = xs._head;
  while ( e->_tail ) { e = e->_tail; }
  return e->_datum;
}

/**
 * Gets from a list its leading sublist of a given size if one exists.
 * @param k a non-negative integer
 * @param xs a list
 * @return a sublist of <var>xs</var> with at most <var>k</var> elements
 * @note returns an empty list if <var>xs</var> is empty
 */
template <typename U>
List<U> take( unsigned k, List<U> const& xs )
{
  using Node = typename List<U>::Node;

  auto from = xs._head;
  if ( k <= 0 || !from ) { return xs; }

  auto ys = List<U>( from->_datum );
  auto to = ys._head;
  while ( --k > 0 && (from = from->_tail) ) {
    to = to->_tail = std::make_shared<Node>( from->_datum );
  }
  return ys;
}

/**
 * Gets what remains after removing a given number of elements from a list.
 * @param k a non-negative integer
 * @param xs a list
 * @return a sublist of <var>xs</var> with at most <var>length-k</var> elements
 * @note returns an empty list if <var>xs</var> has fewer than <var>k</var> elements
 */
template <typename U>
List<U> drop( unsigned k, List<U> const& xs )
{
  auto to = xs._head;
  while ( k-- > 0 && to ) { to = to->_tail; }
  return to;
}

/**
 * Reverses a list.
 * @param xs a list
 * @return a list whose elements are the same as <var>xs</var> but in reverse order
 */
template <typename U>
List<U> reverse( List<U> const& xs )
{
  auto ys = List<U>::EMPTY;
  auto from = xs._head;
  while ( from ) {
    ys = from->_datum | ys;
    from = from->_tail;
  }
  return ys;
}

/**
 * Appends one list to another.
 * @param xs a list
 * @param ys a list
 * @return a list in which the elements of <var>xs</var> precede the elements of <var>ys</var>
 * @note element order is preserved
 */
template <typename U>
List<U> operator+ (List<U> const& xs, List<U> const& ys)
{
  if ( null( xs ) ) { return ys; }
  return List<U>( head( xs ), tail( xs ) + ys );
}

/**
 * Inserts a character string serialization of a list into an output stream.
 * @param os an output stream
 * @param xs a list
 * @return a reference to the output stream
 */
template <typename U>
std::ostream& operator<< (std::ostream& os, List<U> const& xs)
{
  auto n = xs._head;
  if ( n ) {
    os << '[' << n->_datum;
    while ( (n = n->_tail) ) { os << ',' << n->_datum; }
    os << ']';
  }
  return os;
}

} // end namespace prelude

#endif //HPP_PRELUDE_LIST
