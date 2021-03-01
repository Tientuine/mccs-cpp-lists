#ifndef HPP_PRELUDE_LIST
#define HPP_PRELUDE_LIST

#include <iostream>
#include <memory>

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

/** Typed empty list constant - could implement as a method instead. */
template <typename A>
List<A> const EMPTY { nullptr };

/**
 * Internal node structure for a list.
 * @note required so that List can provide a distinct empty-list value
 */
template <typename A>
class Node
{
    friend List<A>;
public:
    /**
     * Make an internal list node from an element and a pointer to a node.
     * @param x  an element
     * @param xs pointer to an existing node (or nullptr)
     */
	Node( A x, List<A> xs = EMPTY<A> ) : head( x ), tail( xs ) {}


    /** The value of an element. */
    A const head;
    /** Node containing the next element. */
    List<A> tail;




    template <typename B> friend B 		 head( List<B> const& );
	template <typename B> friend List<B> tail( List<B> const& );
    template <typename F, typename B> friend auto map( F f, List<B> const& xs ) -> List<decltype( f( head( xs ) ) )>;

};

/**
 * Constructs a list by pre-pending an element to an existing list.
 * @param x an element
 * @param xs a list
 * @return a list with <var>x</var> at the head and <var>xs</var> as the tail
 */
template <typename A>
inline List<A> operator| ( A const& x, List<A> const& xs ) { return std::move( std::make_shared<Node<A>>( x, xs ) ); }
template <typename A>
inline List<A> operator| ( A && x, List<A> && xs ) { return std::move( std::make_shared<Node<A>>( std::forward( x ), std::forward( xs ) ) ); }

// List operations

/**
 * Obtains a new list by applying a function to each element of a list.
 * Converts a list of one type to that of another by applying a specified function.
 * @param f function that takes an element of <var>xs</var> and returns a different type
 * @param xs a list
 * @return a list of elements the same type as the return type of <var>f</var>
 */
template <typename F, typename A>
inline auto map( F f, List<A> const& xs ) -> List<decltype( f( head( xs ) ) )>
{
    using B = decltype( f( head( xs ) ) );

    if ( null( xs ) ) return EMPTY<B>;

    auto ys = std::make_shared<Node<B>>( f( head( xs ) ), EMPTY<B> );
    auto from = xs;
    auto to = ys;
    while ( (from = from->tail) ) {
        to = to->tail = std::make_shared<Node<B>>( f( from->head ), EMPTY<B> );
    }
    return std::move( ys );
    //return f( head( xs ) ) | map( f, tail( xs ) );
}

/**
 * Appends one list to another.
 * @param xs a list
 * @param ys a list
 * @return a list in which the elements of <var>xs</var> precede the elements of <var>ys</var>
 * @note element order is preserved
 */
template <typename A>
inline List<A> operator+ ( List<A> const& xs, List<A> const& ys )
{
  	if ( null( xs ) ) { return ys; }
  	return head( xs ) | tail( xs ) + ys;
}

/**
 * Extracts a sublist of those elements satisfying the given predicate.
 * @param pred a predicate function
 * @param xs a list
 * @return a list containing those elements of <var>xs</var> satisfying <var>pred</var>
 */
template <typename P, typename A>
inline List<A> filter( P pred, List<A> xs )
{
    if ( null( xs ) ) { return EMPTY<A>; }
    auto ys = filter( pred, tail( xs ) );
    auto x = head( xs );
    return pred( x ) ? ( x | ys ) : ys;
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
    return xs->head;
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
  	return xs->tail;
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
  	auto e = xs._head;
  	while ( e->_tail ) { e = e->_tail; }
  	return e->_datum;
}

/**
 * Extracts all except the last element of a list.
 * @param xs a non-empty list
 * @return a list containing all but the last element of <var>xs</var>
 */
template <typename A>
inline List<A> init( List<A> const& xs )
{
  	using Node = typename List<A>::Node;

  	auto  from = xs._head;
  	if ( !from ) { throw xs; }

  	List<A> ys( from->_datum );

  	auto to = ys._head;
  	while ( (from = from->_tail) ) {
    	to = to->_tail = new Node( from->_datum );
  	}
  	return ys;
}

/**
 * Tests whether a list is empty.
 * @param xs a list
 * @return true if <var>xs</var> is empty, false otherwise
 */
template <typename A>
bool null( List<A> const xs ) { return !xs; }

/**
 * Computes the length of a list.
 * @param xs a list
 * @return the number of elements in <var>xs</var>
 * @note This operation has time complexity O(n).
 */
template <typename U>
unsigned length( List<U> xs )
{
  auto n = 0;
  auto e = xs._head;
  while ( e ) { e = e->_tail; ++n; }
  return n;
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
List<A> take( unsigned k, List<A> xs )
{
    if ( null( xs ) ) return xs;
    if ( k == 0 ) return EMPTY<A>;
    return head( xs ) | take( k-1, tail( xs ) );
}

/**
 * Gets what remains after removing a given number of elements from a list.
 * @param k a non-negative integer
 * @param xs a list
 * @return a sublist of <var>xs</var> with at most <var>length-k</var> elements
 * @note returns an empty list if <var>xs</var> has fewer than <var>k</var> elements
 */
template <typename A>
List<A> drop( unsigned k, List<A> xs )
{
    if ( xs )
        while ( k-- )
            xs = tail( xs );
    return xs;
}

/**
 * Reverses a list.
 * @param xs a list
 * @return a list whose elements are the same as <var>xs</var> but in reverse order
 */
template <typename U>
List<U> reverse( List<U> xs )
{
  auto ys = List<U>::EMPTY;
  auto from = xs._head;
  while ( from ) {
    ys = from->_datum | ys;
    from = from->_tail;
  }
  return ys;
}

// Special folds

/**
 * Computes the sum of the elements in a list using operator+.
 * @param pred a predicate function
 * @param xs a list
 * @return a list containing those elements of <var>xs</var> satisfying <var>pred</var>
 */
template <typename A>
A sum( List<A> const& xs )
{
    if ( null( xs ) ) return 0;
    return head( xs ) + sum( tail( xs ) );
}

/**
 * Inserts a character string serialization of a list into an output stream.
 * @param os an output stream
 * @param xs a list
 * @return a reference to the output stream
 */
template <typename A>
std::ostream& operator<< ( std::ostream& os, List<A> xs )
{
    os << '[';
    if ( ! null( xs ) ) {
        os << head( xs );
        while ( (xs = tail( xs )) )
            os << ',' << head( xs );
    }
    os << ']';
    return os;
}

/**
 * Constructs a list by pre-pending an element to an existing list.
 * @param x an element
 * @param xs a list
 * @return a list with <var>x</var> at the head and <var>xs</var> as the tail
 */
//template <typename A>
//List<A> operator| (A x, List<U>&& xs) noexcept { return List<U>(x, std::forward<List<U>>(xs)); }


} // end namespace prelude

#endif //HPP_PRELUDE_LIST

